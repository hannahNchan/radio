#include "si4732.h"
#include "esp_log.h"

static const char* TAG = "SI4732";

SI4732::SI4732() {}

bool SI4732::begin() {
    ESP_LOGI(TAG, "Inicializando SI4732 (simulado)...");
    // Simula que el chip fue detectado correctamente
    return true;
}

void SI4732::setBand(Band band) {
    ESP_LOGI(TAG, "Cambiando banda a: %d", static_cast<int>(band));
    currentBand = band;
}

void SI4732::tuneFrequency(float freq) {
    ESP_LOGI(TAG, "Sintonizando frecuencia: %.1f MHz", freq);
    currentFrequency = freq;
}

void SI4732::volumeUp() {
    if (volume < 10) volume++;
    ESP_LOGI(TAG, "Volumen aumentado: %d", volume);
}

void SI4732::volumeDown() {
    if (volume > 0) volume--;
    ESP_LOGI(TAG, "Volumen disminuido: %d", volume);
}

void SI4732::autoScan() {
    ESP_LOGI(TAG, "Autoescaneo iniciado (simulado)");
    currentFrequency = 94.9;
}

float SI4732::getFrequency() const {
    return currentFrequency;
}

uint8_t SI4732::getVolume() const {
    return volume;
}

bool SI4732::isStereo() const {
    return true; // Simulado como estéreo
}

std::string SI4732::getBandName() const {
    switch (currentBand) {
        case Band::FM: return "FM";
        case Band::AM: return "AM";
        case Band::SW: return "SW";
        default: return "Desconocido";
    }
}
