#include <Adafruit_GFX.h>
#include <Adafruit_ST7789.h>
#include <SPI.h>
#include <SI4735.h>
#include <EncoderButton.h>

#define TFT_CS    10
#define TFT_DC     9
#define TFT_RST    8
// define los pines para la radio
#define RESET_PIN 7

#define AM_FUNCTION 1
#define FM_FUNCTION 0

// Pines del encoder rotativo
#define ENCODER_CLK 5  // D5
#define ENCODER_DT  4  // D4
#define ENCODER_SW  3  // D3

//encoder optico
int encoderValue = 9490; 
volatile int encoderPosition = 0;
const int pinA = 2; // Fase A conectada al pin 2
const int pinB = 3; // Fase B conectada al pin 3

float currentFreq = 94.90; 
SI4735 radio;
Adafruit_ST7789 tft = Adafruit_ST7789(TFT_CS, TFT_DC, TFT_RST);
int lastIncrement = 0;

// Volumen
int lastVolume = -1;

uint16_t currentFrequency;
uint16_t previousFrequency;
uint8_t bandwidthIdx = 0;
const char *bandwidth[] = {"6", "4", "3", "2", "1", "1.8", "2.5"};

volatile int stepAccumulator = 0;

// Colores personalizados
#define COLOR_BACKGROUND ST77XX_BLACK
#define COLOR_BORDER     ST77XX_RED
#define COLOR_TEXT       ST77XX_WHITE
#define COLOR_HIGHLIGHT  ST77XX_BLUE
#define COLOR_SIGNAL     0x07E0 // Verde claro
#define COLOR_BATTERY    0x07E0 // Verde claro
#define COLOR_MODE_BOX   0xFFE0 // Amarillo claro
#define COLOR_ORANGE     0xFD20 // Naranja

// variables
#define IS_MENU_EXPANDED_DEFAULT false

void setup() {
  pinMode(pinA, INPUT_PULLUP);
  pinMode(pinB, INPUT_PULLUP);
  attachInterrupt(digitalPinToInterrupt(pinA), handleEncoder, CHANGE);
  Serial.begin(115200);
  while(!Serial);

  digitalWrite(RESET_PIN, HIGH);
  Serial.println("AM and FM station tuning test.");

  int16_t si4735Addr = radio.getDeviceI2CAddress(RESET_PIN);
  if ( si4735Addr == 0 ) {
    Serial.println("Si473X not found!");
    Serial.flush();
    while (1);
  } else {
    Serial.print("The SI473X / SI474X I2C address is 0x");
    Serial.println(si4735Addr, HEX);
  }

  // Inicializar radio

  delay(500);
  radio.setup(RESET_PIN, FM_FUNCTION);
  // radio.setup(RESET_PIN, -1, 1, SI473X_ANALOG_AUDIO);
  // Starts defaul radio function and band (FM; from 84 to 108 MHz; 103.9 MHz; step 100kHz)
  radio.setFM(8400, 10800, 9490, 10);
  radio.analogPowerUp();
  delay(500);
  radio.setVolume(55);
  int volume = radio.getCurrentVolume();
  adjustRadio();

  // Inicializar pantalla TFT
  tft.init(240, 320, SPI_MODE2);
  tft.setRotation(3);
  tft.fillScreen(COLOR_BACKGROUND);
  drawInterface();
}

void adjustRadio() {
  // rx.getStatus();
  radio.getCurrentReceivedSignalQuality();
  displaySNR();
}

void handleEncoder() {
  if (digitalRead(pinA) == digitalRead(pinB)) {
    stepAccumulator++;
  } else {
    stepAccumulator--;
  }
}

void loop() {
  const int stepsPerTick = 4;  // Cada 4 pasos físicos = 1 paso de frecuencia
  static int lastLogicalStep = 0;

  int logicalStep = encoderPosition / stepsPerTick;

  if (logicalStep != lastLogicalStep) {
    int diff = logicalStep - lastLogicalStep;
    encoderValue += diff * 10; // 10 kHz por tick lógico

    encoderValue = constrain(encoderValue, 8750, 10800);

    radio.setFrequency(encoderValue);
    currentFreq = encoderValue / 100.0;
    displayCurrentFrequencyWithUnit();

    lastLogicalStep = logicalStep;
  }

  int raw = analogRead(A1); // Lee el valor entre 0 y 4095 en ESP32
  int volume = map(raw, 0, 4095, 0, 63); // Ajusta al rango del SI4735

  if (volume != lastVolume) {
    radio.setVolume(volume);
    displayVolume();
    lastVolume = volume;
  }

    const char* menuItems[] = {
      "Band: VHF",
      "Mode: FM",
      "Step: 10",
      "BW:  AUT",
      "Att:  02"
    };
    int selected = 2;
    displayMenu(menuItems, 5, selected, false);
}

void drawInterface() {
  const char* menuItems[] = {
    "Band: VHF",
    "Mode: FM",
    "Step: 10",
    "BW:  AUT",
    "Att:  02"
  };
  int selected = 2;
  displayMenu(menuItems, 5, selected, IS_MENU_EXPANDED_DEFAULT);
  displayCurrentFrequencyWithUnit();
  displayMode("LSB");
  // (radio.getCurrentPilot()) ? "STEREO" : "MONO");
  displayBattery(100);
  displaySignalStrength();
  displayVolume();
  //displayFrequencyScale();
}

void displaySNR() {
  //Serial.print(" [SNR:");
  //Serial.print(radio.getCurrentSNR());
  //Serial.print("dB");
}

void displayMenu(const char* menuItems[], uint8_t itemCount, uint8_t selected, bool isExpanded) {
  tft.drawRect(5, 5, 90, isExpanded ? 145 : 25, COLOR_BORDER); // Altura depende del estado

  // --- Dibuja el encabezado "MENU" ---
  tft.fillRect(5, 5, 90, 18, COLOR_BORDER); 
  tft.setTextColor(COLOR_TEXT, COLOR_BORDER);
  tft.setTextSize(1);

  int16_t x1, y1;
  uint16_t w, h;
  tft.getTextBounds("MENU", 0, 0, &x1, &y1, &w, &h);
  int centeredX = 5 + (90 - w) / 2;
  int centeredY = 5 + (18 - h) / 2;

  tft.setCursor(centeredX, centeredY);
  tft.print("MENU");

  if (!isExpanded) {
    int arrowX = 5 + 45;
    int arrowY = 23;
    tft.fillTriangle(arrowX, arrowY + 5, arrowX - 5, arrowY - 5, arrowX + 5, arrowY - 5, COLOR_BORDER);
    return;
  }

  for (uint8_t i = 0; i < itemCount; i++) {
    if (i == selected) {
      tft.fillRect(10, 30 + i * 20, 80, 18, COLOR_HIGHLIGHT);
      tft.setTextColor(COLOR_TEXT, COLOR_HIGHLIGHT);
    } else {
      tft.setTextColor(COLOR_TEXT, COLOR_BACKGROUND);
    }

    tft.setTextSize(1);
    tft.getTextBounds(menuItems[i], 0, 0, &x1, &y1, &w, &h);
    centeredX = 5 + (90 - w) / 2;

    tft.setCursor(centeredX, 35 + i * 20);
    tft.print(menuItems[i]);
  }

  uint16_t exitY = 30 + itemCount * 20 + 5;
  tft.fillRect(6, exitY, 88, 16, COLOR_BORDER);
  tft.setTextColor(COLOR_TEXT, COLOR_BORDER);
  tft.setTextSize(1);

  const char* exitText = "EXIT ^";
  tft.getTextBounds(exitText, 0, 0, &x1, &y1, &w, &h);
  centeredX = 22 + (50 - w) / 2;
  centeredY = exitY + (15 - h) / 2;

  tft.setCursor(centeredX, centeredY);
  tft.print(exitText);
}

void displayCurrentFrequencyWithUnit() {
  float currentFrequency = radio.getFrequency() / 100.0;
  const char* bandType = radio.isCurrentTuneFM() ? "Mhz" : "Khz";

  // Limpia la pantalla
  tft.fillRect(0, 80, 320, 60, COLOR_BACKGROUND);

  //display in serial
  //Serial.print(String(currentFrequency / 100.0, 2));
  //Serial.print(bandType);
  displayBand(bandType);
  tft.setTextColor(COLOR_TEXT);
  tft.setTextSize(5);

  char buf[10];
  dtostrf(currentFrequency, 7, 2, buf); // 7 espacios, 2 decimales
  
  int16_t x1, y1;
  uint16_t w, h;
  tft.getTextBounds(buf, 0, 0, &x1, &y1, &w, &h);

  int centeredX = (320 - (w + 30)) / 2; // 30px reservados para la palabra (ajustable)
  int centeredY = (240 - h) / 2;

  tft.setCursor(centeredX, 90);
  tft.print(buf);

  int startX = centeredX + w; // Regresar el final del número para poner la palabra
  tft.setTextColor(COLOR_TEXT);
  tft.setTextSize(3); // Un poco más pequeño

  tft.getTextBounds(bandType, 0, 0, &x1, &y1, &w, &h);

  // Subir un poquito para que quede alineado visualmente
  tft.setCursor(startX + 5, 100); // 5px de separación del número
  tft.print(bandType);
}

void displayBand(const char* bandText) {
  tft.setTextColor(COLOR_ORANGE);
  tft.setTextSize(3);
  tft.setCursor(160, 40);
  tft.print(bandText);
}

void displayMode(const char* modeText) {
  tft.drawRect(272, 25, 48, 30, COLOR_MODE_BOX);
  tft.setTextColor(COLOR_TEXT);
  tft.setTextSize(2);
  tft.setCursor(280, 33);
  tft.print(modeText);
}

void displayBattery(uint8_t level) {
  tft.drawRect(290, 5, 25, 12, COLOR_TEXT);
  tft.fillRect(315, 8, 3, 6, COLOR_TEXT);

  uint8_t fill = map(level, 0, 100, 0, 20);
  tft.fillRect(292, 7, fill, 8, COLOR_BATTERY);
}

void displaySignalStrength() {
  tft.setTextColor(COLOR_TEXT);
  tft.setTextSize(1);
  tft.setCursor(200, 1);
  tft.print("SIGNAL");

  int signal = radio.getCurrentRSSI(); // rango 0 - 127

  int baseX = 210;
  int baseY = 30;       // base de las barras
  int maxBars = 6;
  int maxHeight = 20;

  // cada barra representa un nivel de señal
  int levels = map(signal, 0, 127, 0, maxBars * 10); // hasta 60 niveles visuales

  for (int i = 0; i < maxBars; i++) {
    int barHeight = (i + 1) * 4;
    int barWidth = 10;

    int barValue = (i + 1) * 10;
    int barX = baseX + i * (barWidth + 2);
    int barY = baseY - barHeight;

    if (levels >= barValue) {
      // barra completamente activa
      tft.fillTriangle(
        barX, baseY,
        barX + barWidth / 2, barY,
        barX + barWidth, baseY,
        COLOR_SIGNAL
      );
    } else if (levels >= barValue - 10) {
      // barra parcialmente activa (relleno con proporción)
      float pct = (levels - (barValue - 10)) / 10.0;
      int partialHeight = barHeight * pct;
      int partialY = baseY - partialHeight;

      tft.fillTriangle(
        barX, baseY,
        barX + barWidth / 2, partialY,
        barX + barWidth, baseY,
        COLOR_SIGNAL
      );
    } else {
      // borde sin rellenar
      tft.drawTriangle(
        barX, baseY,
        barX + barWidth / 2, barY,
        barX + barWidth, baseY,
        COLOR_TEXT
      );
    }
  }
}

void displayVolume() {
  int volume = radio.getCurrentVolume();
  tft.setTextColor(COLOR_TEXT, COLOR_BACKGROUND);
  tft.setTextSize(2);
  tft.fillRect(230, 140, 80, 20, COLOR_BACKGROUND);
  tft.setCursor(230, 140);
  tft.print("VOL:");
  tft.print(volume);
}

void displayFrequencyScale() {
  uint16_t centerFreq = radio.getFrequency(); // en kHz
  tft.setTextColor(COLOR_TEXT);
  tft.setTextSize(1);

  const int baseY = 220;
  const int majorHeight = 50;
  const int minorHeight = 30;
  const int step = 10; // separación entre líneas (en píxeles)
  const int totalMarks = 320 / step + 2;

  int centerIndex = totalMarks / 2;

  for (int i = 0; i < totalMarks; i++) {
    int offset = i - centerIndex;
    int freqKhz = centerFreq + offset * 5; // cada línea representa 5 kHz
    int x = i * step;

    bool isMajor = (freqKhz % 25 == 0);
    int lineHeight = isMajor ? majorHeight : minorHeight;

    tft.drawFastVLine(x, baseY - lineHeight, lineHeight, COLOR_ORANGE);

    if (isMajor) {
      char buf[10];
      float freqMhz = freqKhz / 1000.0;
      sprintf(buf, "%4.2f", freqMhz);

      int16_t x1, y1;
      uint16_t w, h;
      tft.getTextBounds(buf, 0, 0, &x1, &y1, &w, &h);
      tft.fillRect(x - w / 2, baseY + 5, w, h, COLOR_BACKGROUND);
      tft.setCursor(x - w / 2, baseY + 5);
      tft.print(buf);
    }
  }

  // Indicador central (rojo)
  int x = centerIndex * step;
  tft.fillTriangle(x, baseY - majorHeight + 5, x - 5, baseY - majorHeight - 5, x + 5, baseY - majorHeight - 5, COLOR_BORDER);
  tft.drawFastVLine(x, baseY - majorHeight + 5, majorHeight, COLOR_BORDER);

  // Texto central
  char buf[10];
  sprintf(buf, "%5.3f", centerFreq / 1000.0);
  int16_t x1, y1;
  uint16_t w, h;
  tft.getTextBounds(buf, 0, 0, &x1, &y1, &w, &h);
  tft.fillRect(x - w / 2, baseY - 20, w, h, COLOR_BACKGROUND);
  tft.setCursor(x - w / 2, baseY - 20);
  tft.print(buf);
}

void printScaledText(const char *text, float scale, uint16_t color) {
  int16_t cursor_x = tft.getCursorX();
  int16_t cursor_y = tft.getCursorY();

  while (*text) {
    tft.drawChar(cursor_x, cursor_y, *text, color, ST77XX_BLACK, scale, scale);
    cursor_x += 6 * scale; // avance proporcional al tamaño
    text++;
  }

  // Actualizar el cursor
  // printScaledText("14200", 2.5, ST77XX_WHITE);
  // printScaledText("14200", 2.5, 0x07FF);
  tft.setCursor(cursor_x, cursor_y);
}