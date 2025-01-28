// FILE: DHT22Raw.cpp
// AUTHOR: Custom
// VERSION: 1.0.1
// PURPOSE: DHT22 Sensor library for standalone Arduino (return 32-bit raw data)

#include "DHT_edit.h"
#include <Arduino.h>

DHT22Raw::DHT22Raw()
{
    reset();
}

void DHT22Raw::reset()
{
    for (uint8_t i = 0; i < 5; i++)
    {
        _bits[i] = 0;
    }
}

uint32_t DHT22Raw::read(uint8_t pin)
{
    // Read sensor data
    if (_readSensor(pin, 1) != 0)
    {
        return DHT22RAW_INVALID_VALUE; // Return invalid value on error
    }

    // Combine humidity and temperature into 32-bit value
    uint16_t humidity = (static_cast<uint16_t>(_bits[0]) << 8) | _bits[1];
    uint16_t temperature = (static_cast<uint16_t>(_bits[2]) << 8) | _bits[3];

    // Verify checksum
    uint8_t checksum = _bits[0] + _bits[1] + _bits[2] + _bits[3];
    if (checksum != _bits[4])
    {
        return DHT22RAW_INVALID_VALUE; // Return invalid value on checksum failure
    }

    return (static_cast<uint32_t>(humidity) << 16) | temperature;
}

int DHT22Raw::_readSensor(uint8_t pin, uint8_t wakeupDelay)
{
    uint8_t mask = 128;
    uint8_t idx = 0;

    for (uint8_t i = 0; i < 5; i++)
    {
        _bits[i] = 0;
    }

    // Request sample
    pinMode(pin, OUTPUT);
    digitalWrite(pin, LOW);
    delay(wakeupDelay);
    pinMode(pin, INPUT);
    delayMicroseconds(40);

    // Acknowledge or timeout
    uint16_t loopCnt = DHTLIB_TIMEOUT;
    while (digitalRead(pin) == LOW)
    {
        if (--loopCnt == 0) return -1;
    }

    loopCnt = DHTLIB_TIMEOUT;
    while (digitalRead(pin) == HIGH)
    {
        if (--loopCnt == 0) return -1;
    }

    // Read the output - 40 bits => 5 bytes
    for (uint8_t i = 40; i != 0; i--)
    {
        loopCnt = DHTLIB_TIMEOUT;
        while (digitalRead(pin) == LOW)
        {
            if (--loopCnt == 0) return -1;
        }

        uint32_t t = micros();

        loopCnt = DHTLIB_TIMEOUT;
        while (digitalRead(pin) == HIGH)
        {
            if (--loopCnt == 0) return -1;
        }

        if ((micros() - t) > 40)
        {
            _bits[idx] |= mask;
        }

        mask >>= 1;
        if (mask == 0)
        {
            mask = 128;
            idx++;
        }
    }

    return 0;
}