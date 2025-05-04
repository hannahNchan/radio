#include "display_tft.h"
#include "esp_log.h"
#include "lvgl.h"
#include "esp_lvgl_port.h"
#include "driver/spi_master.h"
#include "esp_lcd_panel_io.h"
#include "esp_lcd_panel_vendor.h"

#define TAG "DisplayTFT"

#define TFT_WIDTH 240
#define TFT_HEIGHT 320

#define TFT_MOSI 23
#define TFT_SCLK 18
#define TFT_CS   5
#define TFT_DC   27
#define TFT_RST  33
#define SPI_HOST_USED SPI2_HOST

static lv_disp_t* disp = nullptr;
static lv_obj_t* label = nullptr;

DisplayTFT::DisplayTFT() {}

void DisplayTFT::begin() {
    ESP_LOGI(TAG, "Inicializando pantalla TFT con LVGL...");

    // 1. Inicializar el bus SPI primero
    spi_bus_config_t buscfg = {
        .mosi_io_num = TFT_MOSI,
        .miso_io_num = -1,
        .sclk_io_num = TFT_SCLK,
        .quadwp_io_num = -1,
        .quadhd_io_num = -1,
        .max_transfer_sz = TFT_WIDTH * TFT_HEIGHT * 2
    };
    ESP_ERROR_CHECK(spi_bus_initialize(SPI_HOST_USED, &buscfg, SPI_DMA_CH_AUTO));

    // 2. Configuración del panel LCD
    esp_lcd_panel_io_handle_t io_handle = NULL;
    esp_lcd_panel_io_spi_config_t io_config = {
        .dc_gpio_num = TFT_DC,
        .cs_gpio_num = TFT_CS,
        .pclk_hz = 40 * 1000 * 1000,
        .lcd_cmd_bits = 8,
        .lcd_param_bits = 8,
        .spi_mode = 0,
        .trans_queue_depth = 10,
    };
    ESP_ERROR_CHECK(esp_lcd_new_panel_io_spi((esp_lcd_spi_bus_handle_t)SPI_HOST_USED, &io_config, &io_handle));

    // 3. Configuración del panel ST7789 (ajusta según tu pantalla)
    esp_lcd_panel_handle_t panel_handle = NULL;
    esp_lcd_panel_dev_config_t panel_config = {
        .reset_gpio_num = TFT_RST,
        .rgb_endian = LCD_RGB_ENDIAN_BGR,
        .bits_per_pixel = 16,
    };
    ESP_ERROR_CHECK(esp_lcd_new_panel_st7789(io_handle, &panel_config, &panel_handle));

    // 4. Inicializar LVGL Port
    const lvgl_port_cfg_t lvgl_cfg = {
        .task_priority = 2,
        .task_stack = 4096,
        .task_affinity = 1,
        .task_max_sleep_ms = 500,
        .timer_period_ms = 5
    };
    ESP_ERROR_CHECK(lvgl_port_init(&lvgl_cfg));

    // 5. Configuración del display para LVGL (nuevo formato en ESP-IDF 5.4.1)
    lvgl_port_display_cfg_t disp_cfg = {
        .io_handle = io_handle,
        .panel_handle = panel_handle,
        .buffer_size = TFT_WIDTH * 40,
        .double_buffer = true,
        .hres = TFT_WIDTH,
        .vres = TFT_HEIGHT,
        .monochrome = false,
    };

    disp = lvgl_port_add_disp(&disp_cfg);
    assert(disp);

    lv_disp_set_rotation(lv_disp_get_default(), LV_DISP_ROT_NONE);

    ESP_LOGI(TAG, "Pantalla TFT con LVGL inicializada correctamente");
    showWelcomeScreen();
}

// [Mantén el resto de tus funciones showWelcomeScreen(), showFrequency(), etc.]