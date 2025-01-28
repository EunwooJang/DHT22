// FILE: DHT_multi.h
// AUTHOR: Custom
// VERSION: 1.0.0
// PURPOSE: Manage communication with multiple DHT sensor slaves

#pragma once

#include <SoftwareSerial.h>

class DHTMulti {
public:
    DHTMulti(uint8_t rxPin, uint8_t txPin, uint8_t slaveAmount);

    // Initialize HC-12 communication
    void begin();

    // Request data from all slaves and return combined results
    // Returns the total combined data of all slaves
    // Each slave contributes 20 bytes of data
    char* getAllSensorData();

private:
    SoftwareSerial HC12;
    uint8_t slaveAmount;

    // Buffer to store combined data
    char* combinedData;

    // Request data from a specific slave
    bool requestSensorData(uint8_t slaveId, char* buffer);

    // Request data resend from a specific slave
    bool requestResendData(uint8_t slaveId, char* buffer);

    // Validate received data
    bool validateReceivedData(const char* data, uint8_t slaveId);
};
