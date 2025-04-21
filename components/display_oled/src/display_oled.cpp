// display_oled.cpp adaptado a espressif/ssd1306 oficial para ESP-IDF + FreeRTOS

#include "display_oled.h"
#include <ssd1306.h>
#include "esp_log.h"
#include <string.h>

#define TAG "DisplayOLED"

#define OLED_I2C_PORT I2C_NUM_0
#define OLED_SDA_PIN 21
#define OLED_SCL_PIN 22
#define OLED_ADDR 0x3C

static ssd1306_handle_t dev = NULL;

DisplayOLED::DisplayOLED() {}

void DisplayOLED::begin() {
    ESP_LOGI(TAG, "Inicializando OLED con ssd1306 oficial...");

    // Inicializa el bus I2C
    if (ssd1306_init(OLED_I2C_PORT, OLED_ADDR, OLED_SDA_PIN, OLED_SCL_PIN) != ESP_OK) {
        ESP_LOGE(TAG, "Fallo al inicializar bus I2C para OLED");
        return;
    }

    // Crea el manejador del OLED
    dev = ssd1306_create(OLED_I2C_PORT, OLED_ADDR);
    if (!dev) {
        ESP_LOGE(TAG, "Fallo al crear instancia de ssd1306");
        return;
    }

    ssd1306_clear(dev);
    ssd1306_refresh_gram(dev);

    showBootMessage();
}

void DisplayOLED::showBootMessage() {
    ssd1306_clear(dev);
    ssd1306_draw_text(dev, 0, 0, "Booting Radio...", 1);
    ssd1306_refresh_gram(dev);
}

void DisplayOLED::showBandInfo(const char* band) {
    ssd1306_draw_text(dev, 0, 2, band, 1); // línea 2 (cada línea es 8 px)
    ssd1306_refresh_gram(dev);
}

void DisplayOLED::showAudioGraph(uint8_t level) {
    for (int x = 0; x < 128; x++) {
        for (int y = 56; y < 64; y++) {
            if (x < level) {
                ssd1306_draw_pixel(dev, x, y, 1);
            } else {
                ssd1306_draw_pixel(dev, x, y, 0);
            }
        }
    }
    ssd1306_refresh_gram(dev);
}
