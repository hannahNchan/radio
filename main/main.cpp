#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "display_tft.h"
#include "esp_log.h"

static const char* TAG = "Main";

// Variables de estado del radio
float currentFrequency = 101.5;
bool stereoStatus = true;
uint8_t volumeLevel = 15;
std::string currentBand = "FM";

extern "C" void app_main(void) {
    // Inicializar display
    static DisplayTFT tft;
    tft.begin();
    
    ESP_LOGI(TAG, "Sistema de radio inicializado");
    tft.showWelcomeScreen();
    vTaskDelay(pdMS_TO_TICKS(2000)); // Mostrar pantalla de bienvenida por 2 segundos

    // Mostrar información inicial
    tft.showFrequency(currentFrequency, stereoStatus);
    tft.showBand(currentBand);
    tft.showVolume(volumeLevel);

    // Simulación de cambios dinámicos
    uint8_t counter = 0;
    while (true) {
        // Ejemplo: Cambiar frecuencia cada 5 segundos
        if(counter % 5 == 0) {
            currentFrequency += 0.2;
            if(currentFrequency > 108.0) currentFrequency = 88.0;
            tft.showFrequency(currentFrequency, stereoStatus);
        }

        // Ejemplo: Cambiar volumen aleatoriamente
        if(counter % 3 == 0) {
            volumeLevel = (volumeLevel + 5) % 100;
            tft.showVolume(volumeLevel);
        }

        counter++;
        ESP_LOGD(TAG, "Contador: %d, Frecuencia: %.1f, Volumen: %d", 
                counter, currentFrequency, volumeLevel);
        
        vTaskDelay(pdMS_TO_TICKS(1000)); // Espera 1 segundo
    }
}