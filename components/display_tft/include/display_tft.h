#pragma once

#include <stdint.h>
#include <string>
#include "lvgl.h"

class DisplayTFT {
public:
    DisplayTFT();

    // Inicializa el display y LVGL
    void begin();

    // Pantalla de bienvenida
    void showWelcomeScreen();

    // Muestra la frecuencia actual y si es estéreo
    void showFrequency(float freq, bool isStereo);

    // Muestra la banda sintonizada
    void showBand(const std::string& band);

    // Muestra el volumen actual
    void showVolume(uint8_t level);
};
