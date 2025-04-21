#pragma once

#include <stdint.h>
#include <string>

class SI4732 {
public:
    enum class Band {
        FM,
        AM,
        SW
    };

    SI4732();

    bool begin();                   // Inicializa el chip (simulado)
    void setBand(Band band);       // Cambia la banda
    void tuneFrequency(float freq);// Sintoniza una frecuencia
    void volumeUp();               // Sube volumen
    void volumeDown();             // Baja volumen
    void autoScan();               // Auto escaneo

    float getFrequency() const;    // Retorna frecuencia actual
    uint8_t getVolume() const;     // Retorna nivel de volumen
    bool isStereo() const;         // Simula si es estéreo
    std::string getBandName() const; // Retorna el nombre de la banda

private:
    Band currentBand = Band::FM;
    float currentFrequency = 94.9f;
    uint8_t volume = 5;
};
