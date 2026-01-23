// Arduino IoT Cloud – automatisch generierter Teil
#include <ArduinoIoTCloud.h>
#include <Arduino_ConnectionHandler.h>

const char SSID[] = "LeonieHotspot";
const char PASS[] = "12345678";

float gewicht_rechts;

void initProperties() {
  ArduinoCloud.addProperty(gewicht_rechts, READ, ON_CHANGE, NULL);
}

WiFiConnectionHandler ArduinoIoTPreferredConnection(SSID, PASS);

// HX711 + Flash-Speicher
#include "HX711.h"
#include <FlashStorage.h>

HX711 scale;

// Pins für die Waage (D2 = DOUT, D3 = SCK)
const int DOUT_PIN = 2;
const int SCK_PIN  = 3;

// Flash-Speicherbereiche
FlashStorage(storeOffset, long);
FlashStorage(storeCalFactor, float);

// Einstellungen
const unsigned long READY_TIMEOUT = 3000;
const int RAW_AVG = 20;
const int SMOOTH_SAMPLES = 5;

// Globale Variablen
long tare_offset = 0;
float calibration_factor = 1.0;
bool firstStart = false;

// Warten, bis der HX711 bereit ist
bool waitForReady(unsigned long timeoutMs) {
  unsigned long start = millis();
  while (!scale.is_ready()) {
    if (millis() - start > timeoutMs) return false;
    delay(5);
  }
  return true;
}

// Mehrfachmessung
long readRawAverage(int samples, int delayMs = 30) {
  long sum = 0;
  for (int i = 0; i < samples; i++) {
    if (!waitForReady(READY_TIMEOUT)) return LONG_MIN;
    sum += scale.read();
    delay(delayMs);
  }
  return sum / samples;
}

// --- Flash speichern ---
void saveToFlash() {
  storeOffset.write(tare_offset);
  storeCalFactor.write(calibration_factor);
}

// --- Flash laden ---
void loadFromFlash() {
  tare_offset = storeOffset.read();
  calibration_factor = storeCalFactor.read();

  // Prüfen, ob Flash leer ist (erste Benutzung)
  if (calibration_factor == 0.0f) {
    firstStart = true;
    tare_offset = 0;
    calibration_factor = 1.0;
  }
}

// --- Referenzgewicht manuell ---
void askForReferenceWeight() {
  Serial.println();
  Serial.println("Bitte Referenzgewicht in Gramm eingeben und Enter drücken:");

  while (!Serial.available()) delay(50);

  String s = Serial.readStringUntil('\n');
  s.trim();
  float known = s.toFloat();

  if (known <= 0.0) {
    Serial.println("Ungültige Zahl. Bitte erneut eingeben.");
    askForReferenceWeight();
    return;
  }

  Serial.println("Messe Rohwerte...");
  long rawAvg = readRawAverage(RAW_AVG, 40);

  if (rawAvg == LONG_MIN) {
    Serial.println("Fehler beim Lesen. Bitte erneut versuchen.");
    askForReferenceWeight();
    return;
  }

  float factor = (rawAvg - (float)tare_offset) / known;

  Serial.print("Kalibrierfaktor = ");
  Serial.println(factor, 6);

  calibration_factor = factor;
  scale.set_scale(calibration_factor);

  saveToFlash();

  Serial.println("Kalibrierung abgeschlossen und gespeichert.");
}

// --- Tare ---
void doTare() {
  Serial.println("Tare läuft... bitte nichts auflegen.");
  scale.tare(10);

  long offset = readRawAverage(10, 30);
  if (offset == LONG_MIN) {
    Serial.println("Fehler: HX711 nicht bereit.");
    return;
  }

  tare_offset = offset;

  saveToFlash();

  Serial.print("Tare abgeschlossen. Offset gespeichert = ");
  Serial.println(tare_offset);
}

// Glättung
float smoothWeight() {
  long sum = 0;

  for (int i = 0; i < SMOOTH_SAMPLES; i++) {
    sum += scale.read();
    delay(10);
  }

  long avg = sum / SMOOTH_SAMPLES;
  return (avg - tare_offset) / calibration_factor;
}

// Setup
void setup() {
  Serial.begin(115200);
  delay(500);

  loadFromFlash();

  initProperties();
  ArduinoCloud.begin(ArduinoIoTPreferredConnection);

  scale.begin(DOUT_PIN, SCK_PIN);

  if (firstStart) {
    Serial.println("Erster Start erkannt → automatische Tare.");
    doTare();
    saveToFlash();
  }

  Serial.println("System bereit.");
  Serial.println("Tare: 't' eingeben");
  Serial.println("Kalibrierung: 'r' eingeben");
  Serial.println("Werte werden mit gespeicherten Einstellungen übertragen.");
}

// Loop
void loop() {
  ArduinoCloud.update();

  // Manuelle Befehle
  if (Serial.available()) {
    String cmd = Serial.readStringUntil('\n');
    cmd.trim();

    if (cmd.equalsIgnoreCase("t")) {
      doTare();
    }

    if (cmd.equalsIgnoreCase("r")) {
      askForReferenceWeight();
    }
  }

  // Werte senden
  gewicht_rechts = smoothWeight();
  Serial.print("Gewicht: ");
  Serial.println(gewicht_rechts);

  delay(200);
}