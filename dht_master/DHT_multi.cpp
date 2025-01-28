// FILE: DHT_multi.cpp
// AUTHOR: Custom
// VERSION: 1.0.0
// PURPOSE: Manage communication with multiple DHT sensor slaves

#include "DHT_multi.h"
#include <Arduino.h>

DHTMulti::DHTMulti(uint8_t rxPin, uint8_t txPin, uint8_t slaveAmount)
    : HC12(rxPin, txPin), slaveAmount(slaveAmount) {
    combinedData = new char[20 * slaveAmount];
    memset(combinedData, 0, 20 * slaveAmount);
}

void DHTMulti::begin() {
    HC12.begin(9600);
}

char* DHTMulti::getAllSensorData() {
    memset(combinedData, 0, 20 * slaveAmount);

    for (uint8_t i = 1; i <= slaveAmount; i++) {
        char buffer[25];
        memset(buffer, 0, sizeof(buffer));

        if (!requestSensorData(i, buffer)) {
            Serial.print("Slave ");
            Serial.print(i);
            Serial.println(" failed. Requesting resend...");
            requestResendData(i, buffer);
        }

        // Append valid data to combinedData
        memcpy(&combinedData[(i - 1) * 20], buffer + 4, 20); // Skip header (4 bytes)
    }

    return combinedData;
}

bool DHTMulti::requestSensorData(uint8_t slaveId, char* buffer) {
    char command[5];
    snprintf(command, 5, "S%dTD", slaveId);

    HC12.write(command);

    if (HC12.available() >= 24) {
        HC12.readBytes(buffer, 24);
        return validateReceivedData(buffer, slaveId);
    }

    return false;
}

bool DHTMulti::requestResendData(uint8_t slaveId, char* buffer) {
    char command[5];
    snprintf(command, 5, "S%dTU", slaveId);

    HC12.write(command);

    if (HC12.available() >= 24) {
        HC12.readBytes(buffer, 24);
        return validateReceivedData(buffer, slaveId);
    }

    return false;
}

bool DHTMulti::validateReceivedData(const char* data, uint8_t slaveId) {
    char expectedHeader[5];
    snprintf(expectedHeader, sizeof(expectedHeader), "D%dTD", slaveId);

    if (strncmp(data, expectedHeader, 4) != 0) {
        return false; // Header mismatch
    }

    // Additional data validation logic could go here if needed

    return true;
}
