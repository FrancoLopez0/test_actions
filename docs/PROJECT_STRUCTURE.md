# Project Structure

## 1. Vision General

Template para **Raspberry Pi Pico 2 (RP2350)** con:

- **Arquitectura estricta de 3 capas** (HAL → Middleware → Application)
- **FreeRTOS SMP** para soporte multicore (2 núcleos)
- **Estrategia de testing dual**: SIL (Software-in-the-Loop) + HIL (Hardware-in-the-Loop)
- Framework de testing **Unity** + mocking con **CMock** via **Ceedling**

```
Application  ──depende──>  Middleware  ──depende──>  HAL  ──depende──>  Pico SDK  ──>  RP2350
```

---

## 2. Árbol de Directorios

```
project-root/
│
├── src/                          # Código fuente del firmware
│   ├── app/                      #   Capa de aplicación (tareas FreeRTOS)
│   ├── hal/                      #   Hardware Abstraction Layer
│   └── middleware/               #   Servicios compartidos HW-independientes
│
├── test/                         # Tests
│   ├── SIL/                      #   Software-in-the-Loop (host nativo)
│   └── HIL/                      #   Hardware-in-the-Loop (target RP2350)
│
├── libs/                         # Librerías internas (placeholder)
├── third-party/                  # Dependencias externas
│   ├── freertos/                 #   FreeRTOS Kernel SMP
│   └── unity/                    #   Unity Test Framework
│
├── tools/                        # Scripts de desarrollo
├── docs/                         # Documentación
├── .github/workflows/            # CI/CD (GitHub Actions)
│
├── CMakeLists.txt                # Build principal (CMake)
├── project.yml                   # Configuración Ceedling (SIL tests)
└── pico_sdk_import.cmake         # Import del Pico SDK
```

---

## 3. Capa Application (`src/app/`)

**Responsabilidad:** Contiene la lógica de alto nivel del firmware. Orquesta tareas FreeRTOS y define el comportamiento del producto.

| Archivo | Propósito |
|---------|-----------|
| `main.c` | Punto de entrada del sistema. Inicializa stdio y el LED onboard, luego llama a `start_firmware()`. |
| `firmware.c` | Orquestador principal. Crea las tareas FreeRTOS, lanza el Core 1 con su propio scheduler. |
| `app_blinky.c` / `app_blinky.h` | Tarea blink. Detecta Pico 2 vs Pico 2W (CYW43), inicializa el LED y lo togglea cada 500ms. |
| `app_motor.c` / `app_motor.h` | Tarea de control de motor. Configura PWM y dirección, alterna potencia entre 50% y 90%. |

**Dependencias:** `middleware`, `freertos`, `pico_multicore`

---

## 4. Capa Middleware (`src/middleware/`)

**Responsabilidad:** Servicios compartidos y lógica independiente del hardware. Toda interacción con hardware real debe delegarse a HAL.

| Archivo | Propósito |
|---------|-----------|
| `mw_led.c` / `mw_led.h` | Control de LED. `init/on/off/toggle` — delega en `hal_gpio`. |
| `motor.c` / `motor.h` | Driver de motor DC. Control de dirección, duty cycle, PWM, validación de pines, manejo de errores. |
| `logging.c` / `logging.h` | Sistema de logging thread-safe con colores ANSI y mutex. Macros `LOGI`, `LOGW`, `LOGE`, `LOGD`. |
| `common_rtos.c` / `common_rtos.h` | Abstracción de RTOS. Wraps FreeRTOS en tipos genéricos (`rtos_mutex_t`, `rtos_sem_t`, `rtos_queue_t`) para facilitar testing y posible reemplazo. |

**Dependencias:** `hal`, `hardware_pwm`, `freertos`

---

## 5. Capa HAL (`src/hal/`)

**Responsabilidad:** Hardware Abstraction Layer. Interacción directa con el Pico SDK. Es la única capa que incluye headers del SDK.

| Archivo | Propósito |
|---------|-----------|
| `hal_common.h` | Include común. Saltea headers del Pico SDK cuando `SIL_TESTING` está definido (para compilación host). |
| `hal_gpio.c` / `hal_gpio.h` | Abstracción de GPIO. `init/write/read/toggle`. Soporte transparente para CYW43 (Pico W/2W). |
| `board_config.h` | Definiciones de pines y PWM (`PIN_PWM`, `PIN_A_1`, etc.). |
| `config.c` / `config.h` | Configuración de hardware: GPIO, PWM, IRQ. |
| `i2c_safe.c` / `i2c_safe.h` | Comunicación I2C thread-safe con cola de requests y semáforo. |

**Dependencias:** `pico_stdlib` (Pico SDK), `pico_cyw43_arch` (solo Pico W/2W)

---

## 6. Librerías Internas (`libs/`)

**Responsabilidad:** Utilidades compartidas que no pertenecen a ninguna capa en particular. Ej: algoritmos, validación de datos, structs genéricos.

**Estado actual:** Placeholder vacío. Agregar aquí cualquier librería interna que se reutilice entre proyectos derivados de este template.

---

## 7. Terceros (`third-party/`)

| Módulo | Versión | Propósito |
|--------|---------|-----------|
| `freertos/` | v202210.01 LTS | FreeRTOS Kernel con soporte SMP para RP2350. Port `RP2350_ARM_NTZ`. Heap: `heap_4`. |
| `unity/` | — | Framework de testing unitario en C. Usado tanto en SIL como en HIL. |

Ambos se integran via CMake como librerías estáticas (`freertos`, `unity`).

---

## 8. Tests (`test/`)

### SIL — Software-in-the-Loop (`test/SIL/`)

- **Build system:** Ceedling (configurado en `project.yml` raíz)
- **Framework:** Unity + CMock (mocks generados automáticamente)
- **Target:** Host nativo (GCC)
- **Propósito:** Validar lógica de middleware sin hardware real

```
ceedling test:all           # ejecutar todos los tests SIL
ceedling test:blinky        # ejecutar un test específico
./tools/run_test.sh blinky  # script wrapper
```

### HIL — Hardware-in-the-Loop (`test/HIL/`)

- **Build system:** CMake + Ninja (cross-compile ARM)
- **Framework:** Unity
- **Target:** RP2350 real, flasheado via picotool
- **Propósito:** Validar HAL e integración con hardware real

```
cmake -S . -B build_hil -DPICO_BOARD=pico2 -DENABLE_HIL_TESTS=ON -GNinja
ninja test_hw && picotool load -f -x build_hil/test/HIL/test_hw.elf
./tools/run_test.sh blinky --hw /dev/ttyACM0   # script wrapper
```

---

## 9. Herramientas (`tools/`)

| Script | Propósito |
|--------|-----------|
| `run_test.sh` | Script bash unificado para SIL y HIL (Linux/Mac). Uso: `./tools/run_test.sh blinky` (SIL) o `./tools/run_test.sh blinky --hw /dev/ttyACM0` (HIL). |
| `run_test.ps1` | PowerShell equivalente (Windows). Uso: `.\tools\run_test.ps1 -TestName blinky` (SIL) o `.\tools\run_test.ps1 -TestName blinky -HW -Port COM3` (HIL). |
| `get_impacted_tests.py` | Script CI que determina qué tests ejecutar según los archivos modificados. Retorna nombres compatibles con Ceedling. |

---

## 10. Documentación (`docs/`)

**Responsabilidad:** Documentación del proyecto. Este archivo (`PROJECT_STRUCTURE.md`) explica la estructura. Agregar aquí:
- Guías de inicio rápido
- Convenciones de código
- Diagramas de arquitectura
- Manuales de referencia

---

## 11. Flujo de Dependencias

```
src/app/ (Application)
  │
  │ depende de:
  ├── src/middleware/   (mw_led, motor, logging, common_rtos)
  ├── third-party/freertos/   (FreeRTOS SMP)
  └── pico_multicore   (Pico SDK)
       │
src/middleware/
  │
  │ depende de:
  ├── src/hal/         (hal_gpio, config, i2c_safe)
  ├── hardware_pwm     (Pico SDK)
  └── third-party/freertos/
       │
src/hal/
  │
  │ depende de:
  ├── pico_stdlib      (Pico SDK)
  └── pico_cyw43_arch  (Pico SDK, solo Pico W/2W)
```

**Regla fundamental:** Las flechas van en una sola dirección. `app/` no puede incluir headers de `hal/` directamente. `middleware/` no puede incluir headers del Pico SDK.

---

## 12. Build System

| Sistema | Para | Configuración | Comando |
|---------|------|---------------|---------|
| **CMake** | Firmware RP2350 + HIL tests | `CMakeLists.txt` raíz | `cmake -S . -B build -DPICO_BOARD=pico2 -GNinja && ninja` |
| **Ceedling** | SIL tests (host) | `project.yml` raíz | `ceedling test:all` |

Ambos coexisten sin conflicto. CMake compila para ARM (cross-compiler), Ceedling compila para host (GCC nativo).
