#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "display_tft.h"
//#include "display_oled.h"

extern "C" void app_main(void) {
    static DisplayTFT tft;
    static DisplayOLED oled;

    // Inicializa pantallas
    tft.begin();
    //oled.begin();

    // Simula contenido en ambas
    tft.showWelcomeScreen();
    tft.showFrequency(101.5, true);
    tft.showBand("FM");
    tft.showVolume(15);

    //oled.showBootMessage();
    //oled.showBandInfo("FM");
    //oled.showAudioGraph(80);

    // Mantén el sistema corriendo
    while (true) {
        vTaskDelay(pdMS_TO_TICKS(1000));
    }
}
