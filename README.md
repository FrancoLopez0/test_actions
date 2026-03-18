# Raspberry Pi Pico 2 (RP2350) Template

A professional, modular template for the Raspberry Pi Pico 2 (RP2350) featuring a 3-layer architecture, FreeRTOS SMP support, and a comprehensive SIL/HIL testing strategy.

## 🏗️ Architecture

The project follows a strict 3-layer architecture to ensure modularity and separation of concerns:

- **`src/app/`**: High-level application logic. Contains the main entry point (`firmware.c`) and tasks.
- **`src/middleware/`**: Shared services and hardware-independent logic (e.g., LED control, sensor processing).
- **`src/hal/`**: Hardware Abstraction Layer. Direct interaction with the Pico SDK 2.x.
- **`libs/`**: Internal utility libraries shared across the project.
- **`third-party/`**: External dependencies (FreeRTOS Kernel, Unity).

## ⚙️ Requirements

- **ARM GCC Toolchain**: `arm-none-eabi-gcc`
- **Pico SDK 2.x**: Ensure `PICO_SDK_PATH` is set in your environment.
- **CMake** & **Ninja** (or Make)
- **Native GCC**: For running SIL tests on the host.

## 🚀 Getting Started

### 1. Build the Firmware (Cross-compilation)

To build the firmware for the RP2350 target:

```bash
mkdir -p build && cd build
cmake .. -DPICO_BOARD=pico2 -GNinja
ninja firmware_binary
```

This will generate `firmware_binary.uf2` in `build/src/app/`.

### 2. Run SIL Tests (Host Native)

Software-in-the-Loop (SIL) tests run on your development machine using a native compiler and mocked hardware layers.

```bash
mkdir -p build_sil && cd build_sil
cmake -S ../test/SIL -B . -GNinja
ninja test_blinky
./test_blinky
```

## 🧪 Testing Strategy

- **SIL (Software-in-the-Loop)**: Validates application and middleware logic on the host machine using **Unity**. Hardware dependencies are mocked in `test/SIL/mock/`.
- **HIL (Hardware-in-the-Loop)**: Targeted tests designed to run on the actual RP2350 hardware to verify HAL and hardware integration.

## 🤖 CI/CD Pipeline

The project includes a GitHub Actions pipeline (`.github/workflows/ci.yml`) that automatically:
- Builds the firmware for the RP2350.
- Compiles and runs SIL tests on the host.
- (Planned) Performs static analysis with `clang-tidy`.

## 📦 Dependencies

- **FreeRTOS SMP**: Symmetric Multi-Processing support for the dual-core RP2350.
- **Unity**: Unit testing framework for C.
