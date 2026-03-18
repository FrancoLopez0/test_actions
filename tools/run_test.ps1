# Usage: .\tools\run_test.ps1 -TestName blinky [-HW] [-Port COM3]

param (
    [Parameter(Mandatory=$true)]
    [string]$TestName,
    
    [switch]$HW,
    
    [string]$Port = "COM3"
)

$Target = "test_$TestName"
$BuildDir = "build_sil"

if ($HW) {
    $BuildDir = "build_hil"
}

Write-Host "🚀 Running $(if($HW){"HIL"}else{"SIL"}) test: $TestName" -ForegroundColor Cyan

if ($HW) {
    # 1. Check for picotool
    if (!(Get-Command picotool -ErrorAction SilentlyContinue)) {
        Write-Host "❌ picotool is required but not found in PATH." -ForegroundColor Red
        exit 1
    }

    Write-Host "🔍 Checking for RP2350 device..." -ForegroundColor Gray
    picotool info | Out-Null # Just to check connectivity

    Write-Host "⚙️  Configuring HIL build..." -ForegroundColor Gray
    cmake -S . -B $BuildDir -DPICO_BOARD=pico2 -GNinja
} else {
    Write-Host "⚙️  Configuring SIL build..." -ForegroundColor Gray
    cmake -S test/SIL -B $BuildDir -GNinja
}

Write-Host "🏗️  Building target: $Target..." -ForegroundColor Gray
cmake --build $BuildDir --target $Target

if ($LASTEXITCODE -eq 0) {
    Write-Host "✅ Build successful." -ForegroundColor Green
    if ($HW) {
        $ElfFile = ".\$BuildDir\test\HIL\$Target.elf"
        Write-Host "⚡ Flashing $ElfFile to RP2350..." -ForegroundColor Yellow
        picotool load -x $ElfFile
        
        if ($LASTEXITCODE -eq 0) {
            Write-Host "⏳ Waiting for serial port $Port to appear..." -ForegroundColor Gray
            $MaxRetries = 20
            $Count = 0
            $PortFound = $false
            while ($Count -lt $MaxRetries) {
                if ([System.IO.Ports.SerialPort]::GetPortNames() -contains $Port) {
                    $PortFound = $true
                    break
                }
                Start-Sleep -Milliseconds 500
                $Count++
            }

            if ($PortFound) {
                Write-Host "📡 Opening serial port $Port at 115200..." -ForegroundColor Cyan
                Write-Host "💡 Press Ctrl+C to stop monitoring." -ForegroundColor Gray
                
                # Simple serial monitor in PowerShell
                $port_serial = New-Object System.IO.Ports.SerialPort $Port, 115200, None, 8, one
                try {
                    $port_serial.Open()
                    while ($port_serial.IsOpen) {
                        if ($port_serial.BytesToRead -gt 0) {
                            $data = $port_serial.ReadExisting()
                            Write-Host $data -NoNewline
                        }
                        Start-Sleep -Milliseconds 100
                    }
                } catch {
                    Write-Host "❌ Could not open $Port. Ensure the port is not in use." -ForegroundColor Red
                } finally {
                    if ($port_serial -ne $null -and $port_serial.IsOpen) {
                        $port_serial.Close()
                    }
                }
            } else {
                Write-Host "❌ Serial port $Port not found after timeout." -ForegroundColor Red
            }
        }
    } else {
        Write-Host "🏃 Executing SIL test..." -ForegroundColor Gray
        & ".\$BuildDir\$Target.exe"
    }
} else {
    Write-Host "❌ Build failed for target: $Target" -ForegroundColor Red
    exit 1
}
