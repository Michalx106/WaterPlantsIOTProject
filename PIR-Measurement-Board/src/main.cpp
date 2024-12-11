#include <Arduino.h>
#include <Adafruit_Sensor.h>
#include "EspNowManager.hpp"
#include "DataStruct.hpp"
#include "SensorReader.hpp"

#define DHT_PIN 18
#define DHT_TYPE DHT11
#define SOIL_MOISTURE_PIN 34
#define VOLTAGE_PIN 34
#define SLEEP_TIME_US 10 * 60 * 100000ULL // Czas uśpienia w mikrosekundach (1 minuta)

uint8_t peerAddress[] = {0x24, 0x6F, 0x28, 0x28, 0x28, 0x28}; // Wprowadź poprawny adres MAC

EspNowManager espNowManager;
SensorData sensorData;
SensorReader sensorReader(DHT_PIN, DHT_TYPE, SOIL_MOISTURE_PIN, VOLTAGE_PIN);

void onDataSent(const uint8_t *macAddr, esp_now_send_status_t status);

void setup() {
  Serial.begin(115200);
  sensorReader.begin();

  if (!espNowManager.begin()) {
    Serial.println("Błąd inicjalizacji ESP-NOW");
    while (true) delay(1000);
  }

  espNowManager.onSend(onDataSent);

  if (!espNowManager.addPeer(peerAddress)) {
    Serial.println("Błąd dodawania odbiorcy");
    while (true) delay(1000);
  }

  sensorData.temperature = sensorReader.readTemperature();
  sensorData.humidity = sensorReader.readHumidity();
  sensorData.soilMoisture = sensorReader.readSoilMoisture();
  sensorData.voltage = sensorReader.readVoltage();

  if (sensorData.soilMoisture > 100) sensorData.soilMoisture = 100;
  if (sensorData.soilMoisture < 0) sensorData.soilMoisture = 0;

  uint8_t buffer[sizeof(SensorData)];
  memcpy(buffer, &sensorData, sizeof(SensorData));

  if (espNowManager.sendData(peerAddress, buffer, sizeof(buffer))) {
    Serial.println("Dane wysłane");
  } else {
    Serial.println("Błąd wysyłania danych");
  }

  Serial.println("Przejście w tryb głębokiego uśpienia...");
  esp_deep_sleep(SLEEP_TIME_US);
}

void loop() {
    // Pętla główna jest nieużywana w trybie uśpienia
}

void onDataSent(const uint8_t *macAddr, esp_now_send_status_t status) {
  Serial.print("Wysłano dane do: ");
  for (int i = 0; i < 6; ++i) {
    Serial.printf("%02X", macAddr[i]);
    if (i < 5) Serial.print(":");
  }
  Serial.print(" Status: ");
  Serial.println(status == ESP_NOW_SEND_SUCCESS ? "Sukces" : "Błąd");
}
