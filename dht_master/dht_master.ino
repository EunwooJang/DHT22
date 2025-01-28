// FILE: main.ino
// AUTHOR: Custom
// VERSION: 1.0.0
// PURPOSE: Main file to request and process data from multiple DHT sensor slaves

#include "DHT_multi.h"

#define RX_PIN 10
#define TX_PIN 11
#define TEMPERATURE_SENSOR_SLAVE_AMOUNT 5

DHTMulti dhtMulti(RX_PIN, TX_PIN, TEMPERATURE_SENSOR_SLAVE_AMOUNT);

void setup() {
    Serial.begin(9600);
    dhtMulti.begin();
}

void loop() {
    char* data = dhtMulti.getAllSensorData();

    // Process and print combined data
    for (uint8_t i = 0; i < TEMPERATURE_SENSOR_SLAVE_AMOUNT; i++) {
        uint32_t rawData;
        memcpy(&rawData, &data[i * 20], 4); // Example: First 4 bytes of each sensor

        float temperature = (rawData & 0xFFFF) / 10.0;
        float humidity = (rawData >> 16) / 10.0;

        Serial.print("Slave ");
        Serial.print(i + 1);
        Serial.print(" - Humidity: ");
        Serial.print(humidity);
        Serial.print("%, Temperature: ");
        Serial.print(temperature);
        Serial.println("°C");
    }

    delay(3000); // Wait for 3 seconds before next request
}
