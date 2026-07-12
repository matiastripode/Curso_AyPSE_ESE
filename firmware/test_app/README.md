# Test App (Unity + ESP-IDF)

Proyecto dedicado a ejecutar tests de firmware en target ESP32-C6 usando Unity.

---

## Que es Unity

Unity es un framework de testing unitario para C, pensado para microcontroladores y sistemas embebidos. Es liviano, portable y no requiere sistema operativo.

ESP-IDF lo integra de forma nativa como componente (`$IDF_PATH/components/unity`) y extiende su uso para target real con:

- registro de casos con `TEST_CASE`
- menu interactivo por UART
- ejecucion por nombre, indice o tags

En este proyecto la funcion central del runner es `unity_run_menu()`, que:

- lista todos los `TEST_CASE` registrados
- permite ejecutar todo con `*` o filtrar por `[tag]`
- reporta `PASS` / `FAIL` por UART
- muestra resumen final de ejecucion

## Objetivo

Este proyecto funciona como harness de test:

- Inicializa el menu interactivo de Unity con `unity_run_menu()`.
- Descubre y ejecuta los `TEST_CASE` registrados por los componentes.
- No contiene logica de producto.

## Estructura actual

```text
firmware/
├── test_app/
│   ├── CMakeLists.txt
│   └── main/
│       ├── CMakeLists.txt
│       └── test_app_main.c
├── board_support/
│   ├── CMakeLists.txt
│   ├── src/
│   └── test/
│       └── test_led.c
└── drivers_hal/
    ├── CMakeLists.txt
    ├── src/
    └── test/
        └── test_gpio_hal.c
```

## Como funciona el proyecto

### Flujo general

```text
test_app/CMakeLists.txt (raiz del proyecto)
|
|-- EXTRA_COMPONENT_DIRS -> middleware, board_support, drivers_hal
|
|-- test_app/main/test_app_main.c      <- app_main() llama unity_run_menu()
|
|-- board_support/CMakeLists.txt       <- agrega board_support/test/test_led.c en modo test
|
`-- drivers_hal/CMakeLists.txt         <- agrega drivers_hal/test/test_gpio_hal.c en modo test
```

### Secuencia de eventos

1. Se selecciona `test_app` como proyecto activo en VS Code / ESP-IDF.
2. CMake incorpora los componentes por `EXTRA_COMPONENT_DIRS`.
3. Cada componente agrega su carpeta `test/` solo para proyectos de test.
4. Al arrancar firmware, ESP-IDF invoca `app_main()`.
5. `app_main()` desactiva TWDT y llama `unity_run_menu()`.
6. El menu queda activo por UART esperando input del usuario.

## Como se integran los tests

- `board_support` y `drivers_hal` son dueños de sus tests.
- Cada componente agrega sus fuentes de test local solo cuando el proyecto activo es de test.
- `test_app` incluye esos componentes y muestra el menu de Unity por UART.

## Incorporacion de testing en la arquitectura

El testing se incorpora como una capacidad nueva de la plataforma con estos principios:

1. Existe un proyecto dedicado de ejecucion de tests: `test_app`.
2. Cada componente mantiene sus pruebas junto a su codigo:
   - `board_support/test/test_led.c`
   - `drivers_hal/test/test_gpio_hal.c`
3. Los componentes agregan fuentes de test solo cuando el proyecto activo es de testing.
4. Los builds de produccion no incluyen fuentes de test.

## Flujo de build y ejecucion

1. Seleccionar la carpeta `test_app` como proyecto activo en VS Code.
2. Configurar/compilar con ESP-IDF.
3. Flashear y abrir monitor.
4. En el menu de Unity:
   - `*` ejecuta todos los tests.
   - `[tag]` filtra por tag.

## Macros de Unity mas usadas

### Afirmaciones basicas

| Macro | Descripcion |
| --- | --- |
| `TEST_ASSERT_TRUE(cond)` | Falla si `cond` es falso |
| `TEST_ASSERT_FALSE(cond)` | Falla si `cond` es verdadero |
| `TEST_ASSERT_NULL(ptr)` | Falla si `ptr` no es NULL |
| `TEST_ASSERT_NOT_NULL(ptr)` | Falla si `ptr` es NULL |

### Comparaciones de enteros

| Macro | Descripcion |
| --- | --- |
| `TEST_ASSERT_EQUAL(expected, actual)` | Compara con `==` |
| `TEST_ASSERT_NOT_EQUAL(expected, actual)` | Compara con `!=` |
| `TEST_ASSERT_GREATER_THAN(threshold, actual)` | `actual > threshold` |
| `TEST_ASSERT_LESS_THAN(threshold, actual)` | `actual < threshold` |
| `TEST_ASSERT_EQUAL_INT(e, a)` | Especifico para `int` |
| `TEST_ASSERT_EQUAL_UINT8(e, a)` | Especifico para `uint8_t` |
| `TEST_ASSERT_EQUAL_UINT16(e, a)` | Especifico para `uint16_t` |
| `TEST_ASSERT_EQUAL_UINT32(e, a)` | Especifico para `uint32_t` |
| `TEST_ASSERT_EQUAL_HEX8(e, a)` | Igual que UINT8 pero muestra hex |
| `TEST_ASSERT_EQUAL_HEX16(e, a)` | Igual que UINT16 pero muestra hex |
| `TEST_ASSERT_EQUAL_HEX32(e, a)` | Igual que UINT32 pero muestra hex |

### Floats

| Macro | Descripcion |
| --- | --- |
| `TEST_ASSERT_FLOAT_WITHIN(delta, expected, actual)` | `abs(actual - expected) <= delta` |
| `TEST_ASSERT_EQUAL_FLOAT(expected, actual)` | Comparacion exacta |

### Arreglos y strings

| Macro | Descripcion |
| --- | --- |
| `TEST_ASSERT_EQUAL_INT_ARRAY(exp, act, len)` | Compara arreglos de enteros |
| `TEST_ASSERT_EQUAL_UINT8_ARRAY(exp, act, len)` | Compara arreglos de bytes |
| `TEST_ASSERT_EQUAL_STRING(exp, act)` | Compara strings |
| `TEST_ASSERT_EQUAL_STRING_LEN(exp, act, len)` | Compara prefijo de largo `len` |

### Control de flujo

| Macro | Descripcion |
| --- | --- |
| `TEST_FAIL()` | Falla sin condicion |
| `TEST_FAIL_MESSAGE("msg")` | Falla con mensaje |
| `TEST_IGNORE()` | Marca como ignorado |
| `TEST_IGNORE_MESSAGE("msg")` | Ignorado con mensaje |

### Registro de casos

```c
TEST_CASE("descripcion legible", "[tag1][tag2]")
{
    // cuerpo del test
}
```

Los tags permiten filtrar desde el menu de Unity.

## Como crear nuevos tests

### 1. Crear el archivo en el componente duenio

Ejemplos:

- `drivers_hal/test/test_gpio_interrupt.c`
- `board_support/test/test_bmp280_proxy.c`

### 2. Agregar el archivo en el CMake del componente

Ejemplo en `drivers_hal/CMakeLists.txt`, dentro del bloque condicional de test:

```cmake
if(PROJECT_NAME STREQUAL "test_app")
    list(APPEND srcs "test/test_gpio_hal.c")
    list(APPEND srcs "test/test_gpio_interrupt.c")
endif()
```

### 3. Escribir los casos

```c
#include "unity.h"
#include "mi_modulo.h"

TEST_CASE("descripcion del caso", "[tag_modulo][subtag]")
{
    TEST_ASSERT_TRUE(true);
}
```

### 4. Compilar, flashear y ejecutar

Usar flujo ESP-IDF del proyecto activo (`test_app`) desde VS Code.

## Como ejecutar los tests

Desde el monitor serie, una vez arrancado el menu de Unity:

| Comando | Efecto |
| --- | --- |
| `*` | Ejecuta todos los `TEST_CASE` |
| `[drivers_hal]` | Solo casos con tag `[drivers_hal]` |
| `[gpio]` | Solo casos con tag `[gpio]` |
| `[drivers_hal][gpio]` | Solo casos que tengan ambos tags |
| numero de caso | Ejecuta un caso por indice |

### Salida esperada

```text
[test_app] Unity menu ready.
[test_app] Enter '*' to run all tests or [tag] to filter.

TEST(GPIOOn and GPIORead return high, drivers_hal) PASS
TEST(GPIOOff and GPIORead return low, drivers_hal) PASS
...
-----------------------
6 Tests 0 Failures 0 Ignored
OK
```

## Cobertura actual

### `drivers_hal` - tag `[drivers_hal][gpio]`

- `GPIOInit` configura pin como salida
- `GPIOOn` y `GPIORead` validan estado alto
- `GPIOOff` y `GPIORead` validan estado bajo
- `GPIOState` maneja alto y bajo
- `GPIOToggle` invierte estado
- `GPIORead` en pin de entrada sin crash

### `board_support` - tag `[board_support][led]`

- `LedsInit` retorna exito
- `LedOn` y `LedOff` para LED_1, LED_2 y LED_3
- `LedToggle` para los tres LEDs
- `LedsOffAll` y `LedsMask`
- manejo de valores invalidos

## Ejemplos de tests para periféricos I2C y SPI

Los siguientes ejemplos son **conceptuales**: muestran la estructura y el razonamiento de cada caso, no código listo para compilar. Los nombres de funciones, registros y constantes deben reemplazarse con los de los HALs y drivers reales del proyecto.

---

### I2C — Patrones comunes

**Patrón 1: verificar identidad del dispositivo (WHO_AM_I)**

La mayoría de los sensores I2C exponen un registro de solo lectura con un valor fijo definido en el datasheet. Leerlo y compararlo contra el valor esperado confirma que el bus está activo, la dirección es correcta y el dispositivo responde.

```
TEST_CASE("I2C sensor responde con WHO_AM_I correcto", "[i2c][sensor]")
{
    // Inicializar el bus I2C con la velocidad y pines del proyecto
    I2CInit(puerto, velocidad)

    // Leer el registro de identificacion del dispositivo
    valor = I2CReadReg(direccion_dispositivo, REG_WHO_AM_I)

    // Comparar contra el valor del datasheet
    TEST_ASSERT_EQUAL_HEX8(VALOR_ESPERADO, valor)
}
```

**Patrón 2: escribir un registro de configuración y leerlo de vuelta**

Escribe un valor en un registro de configuración (modo de operación, rango, rate, etc.) y luego lo lee para verificar que el dispositivo lo almacenó correctamente. Solo aplica a registros de lectura/escritura.

```
TEST_CASE("I2C sensor acepta configuracion y la retiene", "[i2c][sensor]")
{
    I2CInit(puerto, velocidad)

    // Escribir valor de configuracion en un registro R/W
    I2CWriteReg(direccion_dispositivo, REG_CONFIG, VALOR_CONFIG)

    // Leer el mismo registro
    valor_leido = I2CReadReg(direccion_dispositivo, REG_CONFIG)

    // Debe coincidir con lo escrito
    TEST_ASSERT_EQUAL_HEX8(VALOR_CONFIG, valor_leido)
}
```

**Patrón 3: leer datos de medición sin error de transacción**

Para registros de salida (temperatura, aceleración, etc.) el valor exacto depende del entorno físico y no es verificable de forma determinista. El test valida que la transacción I2C completa sin error y que los bytes recibidos tienen una longitud coherente.

```
TEST_CASE("I2C sensor entrega datos de medicion sin error de bus", "[i2c][sensor]")
{
    I2CInit(puerto, velocidad)

    // Leer N bytes del registro de datos de salida (ej: 6 bytes XYZ de un acelerometro)
    resultado = I2CReadReg(direccion_dispositivo, REG_DATA_OUT, N_bytes)

    // Verificar que la transaccion fue exitosa (sin timeout ni NACK)
    TEST_ASSERT_TRUE(resultado == OK)

    // No se verifican valores exactos: dependen del entorno fisico
}
```

---

### SPI — Patrones comunes

**Patrón 1: verificar identidad del dispositivo (READ ID)**

Muchos dispositivos SPI (memorias flash, ADCs, sensores) responden a un comando de lectura de ID con bytes fijos. El primer byte recibido suele ser dummy (el dispositivo aun no respondio); el dato util llega a partir del segundo byte.

```
TEST_CASE("SPI dispositivo responde al comando READ ID", "[spi][dispositivo]")
{
    SPIInit(puerto, modo, velocidad)

    // Armar trama: [CMD_READ_ID, dummy, dummy, ...]
    tx = { CMD_READ_ID, 0x00, 0x00, 0x00 }
    rx = SPITransfer(CS_pin, tx)

    // El ID del fabricante llega en rx[1] (rx[0] es dummy)
    TEST_ASSERT_EQUAL_HEX8(ID_FABRICANTE_ESPERADO, rx[1])
}
```

**Patrón 2: verificar un bit de estado tras un comando de control**

Algunos dispositivos exponen registros de estado que reflejan el efecto de comandos previos (ej: Write Enable Latch en memorias flash, DRDY en ADCs). El test envía el comando y luego lee el registro de estado para verificar que el bit correspondiente cambió.

```
TEST_CASE("SPI comando de habilitacion activa el bit de estado esperado", "[spi][dispositivo]")
{
    SPIInit(puerto, modo, velocidad)

    // Enviar comando de control (ej: WRITE ENABLE, opcode de 1 byte)
    SPITransfer(CS_pin, { CMD_ENABLE })

    // Leer registro de estado: [CMD_READ_STATUS, dummy]
    rx = SPITransfer(CS_pin, { CMD_READ_STATUS, 0x00 })

    // Verificar que el bit de estado quedo seteado
    TEST_ASSERT_TRUE(rx[1] & BIT_ESTADO_ESPERADO)
}
```

**Patrón 3: escribir un registro y leerlo de vuelta**

Para dispositivos con registros de configuración accesibles por SPI (DACs, codecs, drivers de motor). El protocolo suele diferenciar entre comandos de escritura y de lectura por el bit más significativo del primer byte, o por opcodes distintos.

```
TEST_CASE("SPI escritura de registro persiste y puede leerse de vuelta", "[spi][dispositivo]")
{
    SPIInit(puerto, modo, velocidad)

    // Escribir valor en el registro destino
    // Trama tipica: [CMD_WRITE | ADDR, DATA_HIGH, DATA_LOW]
    SPITransfer(CS_pin, { CMD_WRITE | REG_DESTINO, valor_alto, valor_bajo })

    // Leer el mismo registro
    // Trama tipica: [CMD_READ | ADDR, dummy, dummy]
    rx = SPITransfer(CS_pin, { CMD_READ | REG_DESTINO, 0x00, 0x00 })

    // Reconstruir el valor de 16 bits desde los bytes recibidos
    valor_leido = (rx[1] << 8) | rx[2]

    TEST_ASSERT_EQUAL_HEX16(valor_escrito, valor_leido)
}
```

> **Nota:** El protocolo exacto (posición del dummy byte, ancho de datos, formato del comando) varía según el dispositivo. Consultar el datasheet para construir las tramas correctas.

---

## TDD en firmware embebido

### Qué es TDD

Test-Driven Development (TDD) es una disciplina de desarrollo en la que los tests se escriben **antes** que el código de producción. El ciclo es corto e iterativo, y se repite para cada unidad de comportamiento nueva:

```
 ┌─────────────────────────────────────────────────────┐
 │                                                     │
 │   RED → escribir un test que falla                  │
 │    ↓                                                │
 │   GREEN → escribir el mínimo código que lo pase     │
 │    ↓                                                │
 │   REFACTOR → limpiar sin romper los tests           │
 │    ↓                                                │
 │   (repetir para el siguiente comportamiento)        │
 │                                                     │
 └─────────────────────────────────────────────────────┘
```

La idea central es que un test que nunca falló no prueba nada. Ver el test en rojo primero es la única forma de confirmar que está midiendo algo real.

---

### Por qué TDD es difícil en embebidos y cómo adaptarlo

El ciclo clásico de TDD asume feedback rápido: escribís el test, lo corrés en segundos y ves si pasa. En firmware ese ciclo se alarga porque cada iteración implica compilar, flashear y abrir el monitor. Hay dos estrategias para manejarlo:

**Correr lo que se pueda en host.** La lógica pura (parsers, máquinas de estado, cálculos, validaciones) no tiene dependencia de hardware y puede testearse con cualquier framework de C en la PC, sin target. Unity también corre en host. Eso permite ciclos de segundos para la mayor parte del código.

**Separar la HAL del resto.** Si el código de negocio habla contra una interfaz (un conjunto de funciones declaradas en un header), se puede reemplazar la implementación real por una falsa durante el desarrollo en host. La capa HAL en sí se testea en target, pero es pequeña y estable. Todo lo que está encima se testea rápido en host.

Para el código que genuinamente requiere hardware (drivers, inicialización de periféricos, verificación de señales reales), el ciclo TDD en target sigue siendo válido: escribir el test primero, compilar y flashear, confirmar que falla, implementar, compilar y flashear de nuevo, confirmar que pasa.

---

### El ciclo aplicado a un driver nuevo

Suponer que se va a implementar un driver I2C para un sensor hipotético. El proceso TDD sería:

**Paso 1 — RED: escribir el test del primer comportamiento esperado**

Antes de tocar el driver, escribir el caso más básico posible:

```c
TEST_CASE("SensorInit retorna true cuando el bus responde", "[i2c][sensor]")
{
    bool resultado = SensorInit()
    TEST_ASSERT_TRUE(resultado)
}
```

Compilar y flashear. El test falla porque `SensorInit` ni siquiera existe todavía. Ese es el punto de partida correcto.

**Paso 2 — GREEN: implementar lo mínimo para que pase**

Escribir `SensorInit()` con la inicialización del bus y la verificación de presencia del dispositivo (por ejemplo, leyendo el WHO_AM_I). No agregar más lógica de la necesaria para que este test pase.

Compilar, flashear, verificar que el test está en verde.

**Paso 3 — REFACTOR: limpiar**

Si la implementación quedó sucia (magic numbers, estructura confusa), limpiarla ahora que hay un test que respalda el comportamiento. Volver a correr para confirmar que sigue en verde.

**Paso 4 — Repetir para el siguiente comportamiento**

```c
TEST_CASE("SensorRead retorna datos cuando el sensor esta activo", "[i2c][sensor]")
{
    SensorInit()
    datos = SensorRead()
    TEST_ASSERT_TRUE(datos.valido)
}
```

Y así sucesivamente: un comportamiento a la vez, siempre empezando por el test.

---
