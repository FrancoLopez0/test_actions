# Ciclo de Vida de una Feature

Nunca modificamos la rama `main` directamente. Trabajamos con Merge Requests y el pipeline de **GitLab CI/CD** para mantener la calidad del código.

```
main ──[MR]── feat/mi-feature ──[push]── CI/CD ──[verde]── HIL ──[aprobado]── merge a main
```

---

## Fase 1: Desarrollo Local

### 1. Sincroniza y crea tu rama

```bash
git checkout main
git pull
git checkout -b feat/descripcion-de-la-feature
```

**Convención de nombres:**
| Prefijo | Cuándo usarlo |
|---------|---------------|
| `feat/` | Nueva funcionalidad (ej. `feat/lectura-temperatura`) |
| `fix/` | Corrección de bug (ej. `fix/pwm-desincronizado`) |
| `refactor/` | Refactor sin cambios funcionales |
| `docs/` | Cambios en documentación |

### 2. Escribe el código

Respetá la **arquitectura de 3 capas**:

| Capa | Carpeta | Qué va acá |
|------|---------|------------|
| **Application** | `src/app/` | Tareas FreeRTOS, lógica de alto nivel, orquestación. No incluye headers del Pico SDK directamente. |
| **Middleware** | `src/middleware/` | Lógica de negocio independiente del hardware. Drivers de alto nivel (motor, LED, sensores). Toda interacción con el hardware delega en HAL. |
| **HAL** | `src/hal/` | Abstracción del hardware. Única capa que incluye headers del Pico SDK (`pico/stdlib.h`, `hardware/gpio.h`, etc.). |

**Regla de oro:** Las dependencias van en una sola dirección: `app → middleware → hal → Pico SDK`. Una capa nunca debe saltarse a la inferior.

### 3. Correr tests localmente

Antes de subir, verificá que todo compile y los tests SIL pasen:

```bash
# Tests SIL (host, rápidos)
ceedling test:all

# Si agregaste un módulo nuevo, escribí su test SIL primero
ceedling test:blinky   # o el nombre de tu test

# Compilar firmware (cross-compile)
cmake -B build -GNinja && ninja -C build pico2_template
```

---

## Fase 2: Merge Request y CI/CD

### 4. Subí tu rama

```bash
git push origin feat/descripcion-de-la-feature
```

### 5. Abrí el Merge Request en GitLab

- **Base:** `main`
- **Compare:** `feat/descripcion-de-la-feature`
- Agregá una descripción clara del cambio y por qué es necesario.

### 6. El Juez Imparcial (GitLab CI/CD)

Al abrir el MR (o al hacer push), el pipeline de CI se ejecuta automáticamente con 3 etapas:

| Etapa | Qué verifica | Tools |
|-------|-------------|-------|
| 🔍 **Static Analysis** | Sintaxis correcta, buenas prácticas, posibles bugs | `cppcheck` |
| 🏗️ **Build** | El firmware compila correctamente para ARM (RP2350) | `cmake` + `ninja` (ARM GCC) |
| 🧪 **SIL Tests** | La lógica de negocio funciona correctamente en entorno host | `ceedling` (Unity + CMock) |

**Si alguna etapa falla (❌):**

1. Revisá los logs del pipeline en GitLab CI/CD.
2. Corregí el código.
3. Hacé `git commit` y `git push` — el CI se vuelve a ejecutar automáticamente.

El pipeline es **selectivo**: solo ejecuta los tests impactados por los archivos modificados (definido en `tools/get_impacted_tests.py`). Si los cambios afectan archivos de infraestructura (`CMakeLists.txt`, .gitlab-ci.yml, etc.), ejecuta todos los tests.

---

## Fase 3: Pruebas Físicas e Integración

### 7. Test HIL (Hardware-in-the-Loop)

Una vez que el CI/CD pasa en verde, se deben ejecutar pruebas en el hardware real para verificar que los componentes físicos responden como se espera.

```bash
# Compilar y flashear tests HIL
cmake -S . -B build_hil -DPICO_BOARD=pico2 -DENABLE_HIL_TESTS=ON -GNinja
ninja -C build_hil test_hw
picotool load -f -x build_hil/test/HIL/test_hw.elf

# O usando el script wrapper
./tools/run_test.sh blinky --hw /dev/ttyACM0
```

**Documentar los resultados:**
- Mediciones obtenidas (frecuencias, tiempos, valores de sensores)
- Observaciones sobre el comportamiento del hardware
- Capturas de serial output si es relevante
- En caso de no cumplirse lo esperado, crear un `fix/` con las correcciones

### 8. Revisión por un par

Un compañero revisa el código en el PR:
- ✅ ¿Respeta la arquitectura de 3 capas?
- ✅ ¿Los tests cubren los casos borde?
- ✅ ¿El código es legible y está comentado donde hace falta?
- ✅ ¿Las pruebas HIL fueron documentadas?

### 9. Merge a `main`

Una vez aprobado y todo en verde, se hace **merge** del MR a `main`.

```bash
# Opcional: borrar la rama remota y local
git branch -d feat/descripcion-de-la-feature
git push origin --delete feat/descripcion-de-la-feature
```

---

## Diagrama del Ciclo Completo

```
             ┌──────────────────────────────────────┐
             │          1. Desarrollo Local          │
             │  git checkout -b feat/mi-feature      │
             │  Escribir código (app/middleware/hal)  │
             │  ceedling test:all (SIL local)        │
             └──────────┬───────────────────────────┘
                        │ git push
                        ▼
             ┌──────────────────────────────────────┐
             │       2. Merge Request + CI/CD        │
             │  ┌─────────────────────────────────┐  │
             │  │  GitLab CI/CD Pipeline          │  │
             │  │  ├── cppcheck (linting)         │  │
             │  │  ├── cmake + ninja (build ARM)  │  │
             │  │  └── ceedling (SIL tests)       │  │
             │  └──────────┬──────────────────────┘  │
             │             │ ¿Todo verde?            │
             │             ├── Sí → siguiente fase   │
             │             └── No → fix + push       │
             └──────────┬───────────────────────────┘
                        ▼
             ┌──────────────────────────────────────┐
             │     3. Pruebas Físicas (HIL)         │
             │  Flashear RP2350 con picotool        │
             │  Verificar hardware real             │
             │  Documentar resultados               │
             └──────────┬───────────────────────────┘
                        │ ¿OK?
                        ├── Sí → merge a main
                        └── No → git checkout -b fix/...
```

---

## Checklist Antes del MR

- [ ] Los tests SIL pasan localmente (`ceedling test:all`)
- [ ] El firmware compila (`ninja -C build pico2_template`)
- [ ] Los archivos nuevos están en las carpetas correctas (app/middleware/hal)
- [ ] No hay includes que salteen capas (ej. `app/` no incluye `hal/` directamente)
- [ ] Las funciones nuevas tienen tests (SIL para lógica, HIL para hardware)
- [ ] Se ejecutaron tests HIL en hardware real (cuando corresponde)
- [ ] Resultados HIL documentados
