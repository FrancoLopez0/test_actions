# Guía de Inicio Rápido

## Requisitos

| Herramienta | Versión | Para qué |
|-------------|---------|----------|
| `arm-none-eabi-gcc` | — | Compilar firmware para RP2350 |
| `cmake` | ≥ 3.13 | Build system |
| `ninja` (o `make`) | — | Build tool |
| `ruby` | ≥ 3.0 | Ejecutar Ceedling |
| `ceedling` | última | Build + test SIL (gem install ceedling) |
| `picotool` | — | Flashear el RP2350 (solo HIL) |

### Windows (VS Code Extension)

El [Raspberry Pi Pico VS Code Extension](https://marketplace.visualstudio.com/items?itemName=raspberry-pi.raspberry-pi-pico) instala automáticamente el toolchain ARM, CMake, Ninja y picotool en `%USERPROFILE%\.pico-sdk\`.

### Linux (Debian/Ubuntu)

```bash
# Toolchain ARM y herramientas
sudo apt update
sudo apt install -y gcc-arm-none-eabi libnewlib-arm-none-eabi cmake ninja-build ruby-full

# Ceedling
sudo gem install ceedling

# Pico SDK
git clone https://github.com/raspberrypi/pico-sdk.git
cd pico-sdk && git submodule update --init
export PICO_SDK_PATH=$PWD
```

### macOS

```bash
brew install --cask gcc-arm-embedded
brew install cmake ninja ruby picotool
sudo gem install ceedling

# Pico SDK
git clone https://github.com/raspberrypi/pico-sdk.git
cd pico-sdk && git submodule update --init
export PICO_SDK_PATH=$PWD
```

---

## 1. Compilar el Firmware

```bash
# Configurar el proyecto con CMake
mkdir -p build && cd build
cmake .. -DPICO_BOARD=pico2 -GNinja

# Compilar
ninja pico2_template
```

El binario `pico2_template.uf2` se genera en `build/src/app/`.

---

## 2. Flashear (Pico 2 / RP2350)

1. Conecta el Pico 2 manteniendo presionado **BOOTSEL**.
2. Copia `build/src/app/pico2_template.uf2` a la unidad `RPI-RP2`.
3. O via picotool:

```bash
picotool load -f -x build/src/app/pico2_template.elf
```

---

## 3. Ejecutar Tests SIL (Software-in-the-Loop)

Los tests SIL compilan y corren en tu máquina (host nativo), sin necesidad de hardware.

```bash
# Todos los tests
ceedling test:all

# Un test específico (ej: blinky)
ceedling test:blinky

# Usando el script wrapper
./tools/run_test.sh blinky
```

**Windows (PowerShell):**
```powershell
.\tools\run_test.ps1 -TestName blinky
```

> 📖 Los tests SIL validan la lógica de `src/middleware/` usando mocks automáticos generados por CMock.

---

## 4. Ejecutar Tests HIL (Hardware-in-the-Loop)

Los tests HIL compilan para ARM y se flashean al RP2350 real.

```bash
cmake -S . -B build_hil -DPICO_BOARD=pico2 -DENABLE_HIL_TESTS=ON -GNinja
ninja test_hw
picotool load -f -x build_hil/test/HIL/test_hw.elf
```

O usando el script wrapper:

```bash
# Linux/Mac
./tools/run_test.sh blinky --hw /dev/ttyACM0

# Windows
.\tools\run_test.ps1 -TestName blinky -HW -Port COM3
```

---

## 5. Estructura del Proyecto

Para entender la responsabilidad de cada carpeta y la arquitectura de 3 capas, leer:

📄 [`docs/PROJECT_STRUCTURE.md`](PROJECT_STRUCTURE.md)

---

## Resumen de Comandos

| Acción | Comando |
|--------|---------|
| Compilar firmware | `cmake -B build -GNinja && ninja -C build pico2_template` |
| Tests SIL (todos) | `ceedling test:all` |
| Tests SIL (uno) | `ceedling test:blinky` |
| Tests HIL | `cmake -S . -B build_hil -DENABLE_HIL_TESTS=ON -GNinja && ninja -C build_hil test_hw` |
| Flashear | `picotool load -f -x build/src/app/pico2_template.elf` |
| Static analysis | `cppcheck --enable=all -I src/hal -I src/middleware -I src/app src/ libs/` |
