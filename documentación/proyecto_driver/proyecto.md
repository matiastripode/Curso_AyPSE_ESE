

<p align="center">
<img src="./banner.png">
<br>

## Driver de IMU (BMI270) y comunicacion Bluetooth

**Alumno**: Matias Tripode

**Junio 2026**

---

## A. Dispositivo externo seleccionado
Este sensor se selecciono ya que es amplicamente usado en aplicaciones de Wearables, Smart clothing, Realidad Aumentada y deteccion de actividad. Este sensor se suele utilizar en Fitness trackers, wristbands y relojes inteligentes, entre otros. 

### SparkFun BMI270 6 DoF IMU Breakout (Qwiic)

**Fabricante del sensor:** Bosch Sensortec
**Dispositivo comprado:** [sparkfun-6dof-imu-breakout-bmi270-qwiic](https://www.sparkfun.com/sparkfun-6dof-imu-breakout-bmi270-qwiic.html)
**Protocolo:** I2C (hasta 400 kHz), también soporta SPI  

**Recursos:**
- Hoja de datos: Bosch BST-BMI270-DS000 disponible en [bbst-bmi270-ds000.pdf](https://www.bosch-sensortec.com/media/boschsensortec/downloads/datasheets/bst-bmi270-ds000.pdf)
- Driver oficial en C: repositorio BMI270_SensorAPI [Bosch Sensortec en GitHub](https://github.com/boschsensortec/BMI270_SensorAPI)


**Justificación de complejidad de interfaz:**

El BMI270 provee una "interfaz no trivial":

| Característica | Detalle |
|---|---|
| Registros | +125 registros de control, configuración y datos |
| Inicialización | Secuencia obligatoria de 6 pasos (ver abajo) |
| Archivo de config | Requiere cargar 8 KB de firmware interno vía I2C al arranque |
| Modos de operación | Suspend, Low Power, Normal (independientes para accel y gyro) |
| Features inteligentes |contador de pasos, actividad, cualquier-movimiento, no-movimiento, movimientos de muñeca |

**Secuencia de inicialización (requerida por datasheet):**
1. Dummy read (requerido tras Power-On-Reset)
2. Soft reset → escribir `0xB6` en registro CMD (`0x7E`)
3. Esperar ≥2 ms
4. Verificar Chip ID (reg `0x00` → esperado `0x24`)
5. Cargar bloque de configuración (8 KB) en memoria interna
6. Esperar bit `init_ok` en `INTERNAL_STATUS` (`0x21`)
7. Habilitar acelerómetro y giróscopo en `PWR_CTRL` (`0x7D`)

**Pseudo codigo de la App**
La capa de `App` consume los servicios de capas de abstracion sin conocimiento del hardware. La `App` conoce sobre `IMU` pero no sabe que es un `BMI270` o algun otro modelo especifico. La lógica de negocio reside unicamente en la `App`.


```c

void app_main(void) {
    ConfigurarIMU() // configura el BMI270

    while (1) {
        MuestrearIMU();                    // muestrea sensor

        Actividad_IMU    act;
        Orientacion_IMU  ori;
        ObtenerActividadIMU(&act);
        ObtenerOrientacionIMU(&ori);
        uint32_t steps = ObtenerCantidadDePasosIMU();
        EnvierPorBLE(&act, &ori, steps);
        Delay(delay_ms);
    }
}
```

**Propuesta plan de entregas por etapas:**
- **Etapa 1:**  Estructura de drivers. Definicion de los prototipos de función necesarios para inicializar, configurar y manejar el periférico, organizados según la arquitectura en capas del repositorio. 
- **Etapa 2:** Lectura de acelerómetro y giróscopo por I2C → impresión por UART
- **Etapa 3:** Configuración de rangos y conteo de pasos integrado del sensor
- **Etapa 4:** Middleware de clasificación de actividad + BLE → monitor serie o aplicacion generica que lea BLE
- **Etapa 5 (si hay tiempo):** Mostrar actividad en app iOS
