// display_tft.cpp (actualizado para usar LVGL con ESP-IDF + FreeRTOS)

#include "display_tft.h"
#include "esp_log.h"
#include "lvgl.h"
#include "esp_lvgl_port.h"
#include "driver/spi_master.h"
#include "esp_lcd_panel_vendor.h"
#include "esp_lcd_panel_ops.h"
#include <esp_lvgl_port.h>

#define TAG "DisplayTFT"

#define TFT_WIDTH  240
#define TFT_HEIGHT 320

#define TFT_MOSI 23
#define TFT_SCLK 18
#define TFT_CS   5
#define TFT_DC   27
#define TFT_RST  33

static lv_disp_t* disp = NULL;
static lv_obj_t* label = NULL;

DisplayTFT::DisplayTFT() {}

void DisplayTFT::begin() {
    ESP_LOGI(TAG, "Inicializando pantalla TFT con LVGL...");

    const lvgl_port_cfg_t lvgl_cfg = {
        .task_priority = 2,
        .task_stack = 4096,
        .task_affinity = 1,
        .task_max_sleep_ms = 500,
        .timer_period_ms = 5
    };
    ESP_ERROR_CHECK(lvgl_port_init(&lvgl_cfg));

    const lvgl_port_spi_cfg_t lvgl_spi_cfg = {
        .spi_host = SPI2_HOST,
        .miso_io_num = -1,
        .mosi_io_num = TFT_MOSI,
        .sclk_io_num = TFT_SCLK,
        .max_transfer_sz = TFT_WIDTH * TFT_HEIGHT * sizeof(uint16_t)
    };

    const lvgl_port_display_cfg_t disp_cfg = {
        .io_type = LVGL_PORT_IO_SPI,
        .spi = {
            .cs_io_num = TFT_CS,
            .dc_io_num = TFT_DC,
            .spi_mode = 0,
            .pclk_hz = 40 * 1000 * 1000,
        },
        .panel = {
            .vendor_config = {
                .chip_info = {
                    .interface = ESP_LCD_INTERFACE_SPI,
                    .vendor = ESP_LCD_VENDOR_ST7789,
                    .color_space = ESP_LCD_COLOR_SPACE_RGB,
                    .bits_per_pixel = 16,
                    .reset_gpio_num = TFT_RST,
                }
            }
        },
        .buffer_size = TFT_WIDTH * TFT_HEIGHT,
        .double_buffer = true,
        .hres = TFT_WIDTH,
        .vres = TFT_HEIGHT,
        .monochrome = false,
    };

    disp = lvgl_port_add_disp(&lvgl_spi_cfg, &disp_cfg);
    assert(disp);

    lv_disp_t* default_disp = lv_disp_get_default();
    lv_disp_set_rotation(default_disp, LV_DISP_ROT_NONE);

    ESP_LOGI(TAG, "Pantalla TFT con LVGL inicializada correctamente");

    // Mostrar pantalla inicial
    showWelcomeScreen();
}

void DisplayTFT::showWelcomeScreen() {
    lv_obj_clean(lv_scr_act());
    lv_obj_t* label = lv_label_create(lv_scr_act());
    lv_label_set_text(label, "Radio Receptor ESP32\nInicializando...");
    lv_obj_align(label, LV_ALIGN_CENTER, 0, 0);
}

void DisplayTFT::showFrequency(float freq, bool isStereo) {
    lv_obj_clean(lv_scr_act());
    char buffer[64];
    snprintf(buffer, sizeof(buffer), "Frecuencia: %.1f MHz\nModo: %s", freq, isStereo ? "Stereo" : "Mono");
    label = lv_label_create(lv_scr_act());
    lv_label_set_text(label, buffer);
    lv_obj_align(label, LV_ALIGN_TOP_LEFT, 10, 10);
}

void DisplayTFT::showBand(const std::string& band) {
    lv_obj_clean(lv_scr_act());
    std::string txt = "Banda: " + band;
    label = lv_label_create(lv_scr_act());
    lv_label_set_text(label, txt.c_str());
    lv_obj_align(label, LV_ALIGN_TOP_LEFT, 10, 40);
}

void DisplayTFT::showVolume(uint8_t level) {
    lv_obj_clean(lv_scr_act());
    char buffer[32];
    snprintf(buffer, sizeof(buffer), "Volumen: %d", level);
    label = lv_label_create(lv_scr_act());
    lv_label_set_text(label, buffer);
    lv_obj_align(label, LV_ALIGN_TOP_LEFT, 10, 70);
}
