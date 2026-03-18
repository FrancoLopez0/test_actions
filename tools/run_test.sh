#!/bin/bash

# Usage: ./tools/run_test.sh <test_name> [--hw] [port]
# Example: ./tools/run_test.sh blinky --hw /dev/ttyACM0

if [ -z "$1" ]; then
    echo "Usage: $0 <test_name> [--hw] [serial_port]"
    echo "Example: $0 blinky --hw /dev/ttyACM0"
    exit 1
fi

TEST_NAME=$1
HIL_MODE=false
SERIAL_PORT=${3:-"/dev/ttyACM0"}
BUILD_DIR="build_sil"
TARGET="test_${TEST_NAME}"

if [ "$2" == "--hw" ]; then
    HIL_MODE=true
    BUILD_DIR="build_hil"
fi

echo "🚀 Running $([ "$HIL_MODE" = true ] && echo "HIL" || echo "SIL") test: $TEST_NAME"

if [ "$HIL_MODE" = true ]; then
    # 1. Check for picotool
    if ! command -v picotool &> /dev/null; then
        echo "❌ picotool is required but not found in PATH."
        exit 1
    fi

    echo "🔍 Checking for RP2350 device..."
    if ! picotool info &> /dev/null; then
        echo "⚠️  No RP2350 device detected. Please connect it in BOOTSEL mode or via reset."
        # We continue anyway as some systems might need the build to finish first
    fi

    # Use PICO_BOARD from environment or default to pico2
    BOARD=${PICO_BOARD:-"pico2"}
    echo "⚙️  Configuring HIL build for board: $BOARD..."
    cmake -S . -B "$BUILD_DIR" -DPICO_BOARD="$BOARD" -GNinja
else
    echo "⚙️  Configuring SIL build..."
    cmake -S test/SIL -B "$BUILD_DIR" -GNinja
fi

echo "🏗️  Building target: $TARGET..."
cmake --build "$BUILD_DIR" --target "$TARGET"

if [ $? -eq 0 ]; then
    echo "✅ Build successful."
    if [ "$HIL_MODE" = true ]; then
        ELF_FILE="$BUILD_DIR/test/HIL/${TARGET}.elf"
        echo "⚡ Flashing $ELF_FILE to RP2350..."
        picotool load -x "$ELF_FILE"
        
        if [ $? -eq 0 ]; then
            echo "⏳ Waiting for serial port $SERIAL_PORT to appear..."
            MAX_RETRIES=10
            COUNT=0
            while [ ! -e "$SERIAL_PORT" ] && [ $COUNT -lt $MAX_RETRIES ]; do
                sleep 0.5
                COUNT=$((COUNT + 1))
            done

            if [ ! -e "$SERIAL_PORT" ]; then
                echo "🔍 Specified port $SERIAL_PORT not found. Attempting auto-detection..."
                AUTO_PORT=$(ls /dev/ttyACM* 2>/dev/null | head -n 1)
                if [ -n "$AUTO_PORT" ]; then
                    echo "✨ Found device at $AUTO_PORT"
                    SERIAL_PORT=$AUTO_PORT
                fi
            fi

            if [ -e "$SERIAL_PORT" ]; then
                echo "📡 Opening serial port $SERIAL_PORT..."
                echo "💡 Press Ctrl+A then Q to exit (minicom) or Ctrl+C if using cat."
                
                # Try to use minicom, tio, or fallback to cat
                if command -v minicom &> /dev/null; then
                    minicom -D "$SERIAL_PORT" -b 115200
                elif command -v tio &> /dev/null; then
                    tio "$SERIAL_PORT"
                else
                    echo "⚠️  No minicom/tio found. Falling back to 'cat'."
                    stty -F "$SERIAL_PORT" 115200 raw -echo
                    cat "$SERIAL_PORT"
                fi
            else
                echo "❌ Serial port $SERIAL_PORT not found after timeout. Ensure USB CDC is enabled in your code."
            fi
        fi
    else
        echo "🏃 Executing SIL test..."
        "./$BUILD_DIR/$TARGET"
    fi
else
    echo "❌ Build failed for target: $TARGET"
    exit 1
fi
