# Usage: .\tools\run_test.ps1 -TestName motor [-HW] [-Port COM7] [-ShowLogs]
param (
    [Parameter(Mandatory=$true)]
    [string]$TestName,
    [switch]$HW,
    [string]$Port = "COM3",
    [switch]$ShowLogs,
    
    # Parameterized paths (Relative to User Profile)
    [string]$CMakeCmd = "$HOME\.pico-sdk\cmake\v3.31.5\bin\cmake.exe",
    [string]$NinjaPath = "$HOME\.pico-sdk\ninja\v1.12.1\ninja.exe",
    [string]$PicotoolCmd = "$HOME\.pico-sdk\picotool\2.2.0-a4\picotool\picotool.exe"
)

# Ensure UTF8 for special characters
[Console]::OutputEncoding = [System.Text.Encoding]::UTF8

$Target = "test_$TestName"
$BuildDir = if ($HW) { "build_hil" } else { "build_sil" }

# 1. Build
Write-Host "--- Building ${Target} ---" -ForegroundColor Cyan
if ($HW) {
    & $CMakeCmd -S . -B $BuildDir -DPICO_BOARD=pico2 -GNinja "-DCMAKE_MAKE_PROGRAM=$NinjaPath" -DENABLE_HIL_TESTS=ON | Out-Null
} else {
    & $CMakeCmd -S test/SIL -B $BuildDir -GNinja "-DCMAKE_MAKE_PROGRAM=$NinjaPath" | Out-Null
}
& $CMakeCmd --build $BuildDir --target $Target | Out-Null

if ($LASTEXITCODE -ne 0) {
    Write-Host "Error: Build failed." -ForegroundColor Red
    exit 1
}

# 2. Flash and Run
if ($HW) {
    $ElfFile = "$BuildDir\test\HIL\$Target.elf"
    Write-Host "Flashing ${ElfFile}..." -ForegroundColor Yellow
    & $PicotoolCmd load -f -x $ElfFile | Out-Null
    
    if ($LASTEXITCODE -eq 0) {
        Write-Host "Waiting for ${Port} (Timeout: 20s)..." -ForegroundColor Gray
        
        $portObj = New-Object System.IO.Ports.SerialPort
        $portObj.PortName = $Port
        $portObj.BaudRate = 115200
        $portObj.DtrEnable = $true
        $portObj.RtsEnable = $true

        $attempts = 0
        while (-not $portObj.IsOpen -and $attempts -lt 40) {
            try { $portObj.Open() } catch { Start-Sleep -Milliseconds 500; $attempts++; Write-Host "." -NoNewline }
        }

        if (-not $portObj.IsOpen) {
            Write-Host "`nError: Could not open ${Port}." -ForegroundColor Red
            $available = [System.IO.Ports.SerialPort]::GetPortNames()
            Write-Host "Available ports: [ $($available -join ', ') ]" -ForegroundColor Gray
            exit 1
        }

        Write-Host "`nConnected. Monitoring tests...`n" -ForegroundColor Green
        
        $passCount = 0
        $failCount = 0

        try {
            while ($portObj.IsOpen) {
                if ($portObj.BytesToRead -gt 0) {
                    $line = $portObj.ReadLine().Trim()
                    
                    # Unity Format: "path/to/file.c:line:test:PASS" or "FAIL"
                    if ($line -match "(.+\.c):(\d+):(.+):(PASS|FAIL)(.*)") {
                        # Extract only filename (leaf) from the potential path
                        $fileFull = $Matches[1]
                        $file = Split-Path $fileFull -Leaf
                        
                        $fline = $Matches[2]
                        $test = $Matches[3]
                        $status = $Matches[4]
                        
                        $output = ".. ${file}:${fline}:${test}:${status} .."
                        if ($status -eq "PASS") {
                            Write-Host $output -ForegroundColor Green
                            $passCount++
                        } else {
                            Write-Host $output -ForegroundColor Red
                            $failCount++
                        }
                    } elseif ($line -match "Tests (\d+) Failures") {
                        Write-Host "`n${line}" -ForegroundColor Yellow
                        break
                    } elseif ($ShowLogs) {
                        Write-Host $line -ForegroundColor Gray
                    }
                }
                Start-Sleep -Milliseconds 10
            }
        } finally {
            $portObj.Close()
            Write-Host "`n--- Final Summary ---" -ForegroundColor Cyan
            Write-Host "Tests Passed: ${passCount}" -ForegroundColor Green
            Write-Host "Tests Failed: ${failCount}" -ForegroundColor Red
        }
    }
} else {
    Write-Host "Executing SIL test..." -ForegroundColor Cyan
    & ".\$BuildDir\$Target.exe"
}
