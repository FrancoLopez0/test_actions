# Raspberry Pi Pico 2 (RP2350) Template

Template modular para la **Raspberry Pi Pico 2 (RP2350)** con arquitectura de 3 capas, **FreeRTOS SMP** (multinúcleo), y una estrategia de testing dual **SIL/HIL**.

---

## 🏗️ Arquitectura

El proyecto sigue una arquitectura estricta de 3 capas con dependencia unidireccional:

```
src/app/  →  src/middleware/  →  src/hal/  →  Pico SDK  →  RP2350
```

| Capa | Carpeta | Responsabilidad |
|------|---------|----------------|
| **Application** | `src/app/` | Tareas FreeRTOS y lógica de alto nivel |
| **Middleware** | `src/middleware/` | Servicios independientes del hardware (LED, motor, logging, RTOS wrapper) |
| **HAL** | `src/hal/` | Hardware Abstraction Layer — única capa que toca el Pico SDK |

📄 **[Ver `docs/PROJECT_STRUCTURE.md`](docs/PROJECT_STRUCTURE.md)** — documentación completa de cada carpeta y archivo.

---

## ⚙️ Requisitos

| Herramienta | Versión | Para qué |
|-------------|---------|----------|
| `arm-none-eabi-gcc` | — | Compilar firmware para RP2350 |
| `cmake` | ≥ 3.13 | Build system |
| `ninja` (o `make`) | — | Build tool |
| `ruby` | ≥ 3.0 | Ejecutar Ceedling |
| `ceedling` | última | Build + ejecución de tests SIL |
| `picotool` | — | Flashear el RP2350 (solo HIL) |

📄 **[Ver `docs/QUICKSTART.md`](docs/QUICKSTART.md)** — guía de instalación detallada para Windows, Linux y macOS.

---

## 🚀 Primeros pasos

### 1. Compilar el firmware

```bash
mkdir -p build && cd build
cmake .. -DPICO_BOARD=pico2 -GNinja
ninja pico2_template
```

El binario `pico2_template.uf2` se genera en `build/src/app/`.

### 2. Tests SIL (Software-in-the-Loop)

Ejecutan en tu máquina sin necesidad de hardware real:

```bash
ceedling test:all                    # todos los tests
ceedling test:blinky                 # un test específico
./tools/run_test.sh blinky           # script wrapper (Linux/Mac)
.\tools\run_test.ps1 -TestName blinky  # script wrapper (Windows)
```

### 3. Tests HIL (Hardware-in-the-Loop)

Ejecutan en el RP2350 real via picotool:

```bash
cmake -S . -B build_hil -DPICO_BOARD=pico2 -DENABLE_HIL_TESTS=ON -GNinja
ninja -C build_hil test_hw
picotool load -f -x build_hil/test/HIL/test_hw.elf
```

📄 **[Ver `docs/QUICKSTART.md`](docs/QUICKSTART.md)** — más ejemplos y variantes.

---

## 🔄 Ciclo de desarrollo

Trabajamos con Merge Requests y **GitLab CI/CD**. Nunca se modifica `main` directamente.

```
git checkout -b feat/mi-feature
# ... escribir código respetando las 3 capas ...
ceedling test:all                    # tests SIL locales
git push origin feat/mi-feature     # → CI/CD automático
# MR → revisión → tests HIL → merge a main
```

📄 **[Ver `docs/DEV_CYCLE.md`](docs/DEV_CYCLE.md)** — ciclo completo con fases, ejemplos y checklist.

---

## 🤖 Pipeline CI/CD (GitLab)

El archivo `.gitlab-ci.yml` se ejecuta automáticamente en cada Push y Pull Request:

| Etapa | Herramienta | Verifica |
|-------|-------------|----------|
| 🔍 Static Analysis | `cppcheck` | Sintaxis y buenas prácticas |
| 🏗️ Build (ARM) | `cmake` + `ninja` | El firmware compila para RP2350 |
| 🧪 SIL Tests | `ceedling` (Unity + CMock) | La lógica de negocio funciona correctamente |

El pipeline es **selectivo**: solo ejecuta los tests impactados por los archivos modificados.

---

## 📦 Dependencias incluidas

| Módulo | Versión | Propósito |
|--------|---------|-----------|
| **FreeRTOS SMP** | v202210.01 LTS | Kernel con soporte multinúcleo para RP2350 |
| **Unity** | — | Framework de testing unitario en C (SIL y HIL) |
| **CMock** | vía Ceedling | Generación automática de mocks para tests SIL |
