// display_oled.h (actualizado para usar el driver oficial espressif/ssd1306)

#pragma once

#include <stdint.h>

class DisplayOLED {
public:
    DisplayOLED();
    void begin();
    void showBootMessage();
    void showBandInfo(const char* band);
    void showAudioGraph(uint8_t level);
};
