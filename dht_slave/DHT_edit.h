#pragma once

#include <Arduino.h> // Arduino 데이터 타입 및 함수 사용

#define DHT22RAW_INVALID_VALUE 0xFFFFFFFF // 32-bit NaN equivalent
const int DHTLIB_TIMEOUT = (F_CPU / 40000);

class DHT22Raw
{
public:
    DHT22Raw();

    // Reads the raw 32-bit data from DHT22
    // Returns:
    //   - 32-bit combined humidity (16-bit) and temperature (16-bit) data
    //   - DHT22RAW_INVALID_VALUE if checksum fails or timeout occurs
    uint32_t read(uint8_t pin);

private:
    uint8_t _bits[5];
    
    void reset();
    
    int _readSensor(uint8_t pin, uint8_t wakeupDelay);
};
