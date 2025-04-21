# Radio Receptor con ESP32 y SI4732-A10

Proyecto para construir un receptor de radio de onda corta (SW), media (MW) y frecuencia modulada (FM) usando el microcontrolador **ESP32**, el chip **SI4732-A10**, y múltiples periféricos SPI/I2C.

---

## 📐 Estructura del proyecto

```
RadioReceptorESP32/
├── components/
│   ├── si4732/
│   │   ├── include/
│   │   └── src/
│   ├── display_tft/
│   │   ├── include/
│   │   └── src/
│   ├── display_oled/
│   │   ├── include/
│   │   └── src/
│   ├── controls/              # Encoder, botones, potenciómetro
│   │   ├── include/
│   │   └── src/
│   ├── audio/
│   │   ├── include/
│   │   └── src/
│   └── ui_logic/              # Coordinación de interfaz y lógica de usuario
│       ├── include/
│       └── src/
├── main/
│   └── main.c
├── sdkconfig.defaults
└── README.md
```

---

## 🧩 Componentes y funcionalidades

| Componente            | Descripción                                                                 |
|-----------------------|-----------------------------------------------------------------------------|
| **ESP32**             | Microcontrolador principal                                                  |
| **SI4732-A10**        | Receptor AM/FM/SW con I²C                                                   |
| **Pantalla TFT**      | 2.4" SPI 240x320, muestra frecuencia, banda, estado                         |
| **Pantalla OLED**     | 0.96" I2C 128x64, muestra la banda y una gráfica de volumen/actividad       |
| **Encoder EC11**      | Permite sintonizar frecuencia                                               |
| **Potenciómetro**     | Control de volumen vía entrada ADC                                          |
| **6 botones**         | Cambio de banda, búsqueda, favoritos, modo estéreo/mono, etc.               |
| **Botón de encendido**| Controla encendido general del sistema                                     |

---

## 🖥️ Interfaz

```
TFT Display (Main)
┌─────────────────────────────┐
│        95.70 MHz (FM)       │  ← Frecuencia y modo
│        Stereo | RDS         │  ← Indicadores
│ ┌─────────────────────────┐ │
│ │                         │ │  ← Barras, texto u ondas
│ └─────────────────────────┘ │
└─────────────────────────────┘

OLED Display
┌───────────────┐
│     FM Band   │
│ ▓▓▓▓▓▓▓       │  ← Volumen/visualización audio
└───────────────┘
```

---

## ⚙️ Compilación del proyecto

Este proyecto usa el framework **ESP-IDF**.

### Prerrequisitos

- macOS/Linux/Windows con:
  - Python 3.x
  - [ESP-IDF](https://docs.espressif.com/projects/esp-idf/en/latest/esp32/get-started/)

### Clonación y configuración

```bash
git clone https://github.com/tu_usuario/RadioReceptorESP32.git
cd RadioReceptorESP32
idf.py set-target esp32
idf.py menuconfig
```

### Compilar y flashear

```bash
idf.py build
idf.py -p /dev/ttyUSB0 flash monitor
```

---

## 📡 Funcionalidades clave

- Sintonización manual con encoder EC11
- Auto-búsqueda y salto entre bandas (SW/MW/FM)
- Visualización de volumen
- Detección de estéreo (FM)
- Almacenamiento de favoritos (próximamente)
- Control de encendido físico
- Visualización dual (OLED y TFT)
- Todo manejado con FreeRTOS

---

## 🛠️ Licencia

MIT License
© 2025 TuNombre

---

## ✉️ Contacto

Para dudas, mejoras o contribuciones:

- Correo: hannah@noelia.mx
- GitHub: [@tu_usuario](https://github.com/hannahNchan)
