#include "DHT_multi.h"
#include <Arduino.h>
#include <SoftwareSerial.h>

extern SoftwareSerial hc12; // 메인에서 선언된 hc12 사용

DHTMulti::DHTMulti(uint8_t slaveAmount)
  : slaveAmount(slaveAmount) {
  combinedData = new char[20 * slaveAmount];
  memset(combinedData, 0, 20 * slaveAmount);
}

char* DHTMulti::getAllSensorData() {
  memset(combinedData, 0, 20 * slaveAmount);

  for (uint8_t i = 1; i <= slaveAmount; i++) {
    
    char buffer[24];
    
    memset(buffer, 0, sizeof(buffer));
    
    if (!requestSensorData(i, buffer)) {
        
        if (!requestResendData(i, buffer)) {
            // 두 개의 if문이 모두 해당되면 0으로 채움
            memset(&combinedData[(i - 1) * 20], 0, 20);
        } else {
            // 첫 번째 if만 해당되거나 두 번째가 해당되지 않으면 buffer 데이터 복사
            memcpy(&combinedData[(i - 1) * 20], buffer + 4, 20);
        }
    } else {
      
      // 첫 번째 if문이 해당되지 않으면 buffer 데이터 복사
      memcpy(&combinedData[(i - 1) * 20], buffer + 4, 20);
    }

  }

  return combinedData;
}

// 데이터 요청
bool DHTMulti::requestSensorData(uint8_t slaveId, char* buffer) {
  char command[5];
  snprintf(command, 5, "S%dTD", slaveId);
  hc12.write(command);
  hc12.flush();

    unsigned long startTime = millis();
    while ((millis() - startTime) < 200) { // 200ms 대기
      if (hc12.available() >= 24) {
        hc12.readBytes(buffer, 24);
        return validateReceivedData(buffer, slaveId);
      }
    }
    
    while (hc12.available()) {
      hc12.read();
    }

  return false; // 타임아웃
}

// 데이터 재요청
bool DHTMulti::requestResendData(uint8_t slaveId, char* buffer) {
  char command[5];
  snprintf(command, 5, "S%dTU", slaveId);

  hc12.write(command);
  hc12.flush();

  unsigned long startTime = millis();
  while ((millis() - startTime) < 200) { // 200ms 대기
    if (hc12.available() >= 24) {
      hc12.readBytes(buffer, 24);
      return validateReceivedData(buffer, slaveId);
    }
  }

  while (hc12.available()) {
      hc12.read();
  }

  return false; // 타임아웃
}

// 데이터 유효성 검사
bool DHTMulti::validateReceivedData(const char* data, uint8_t slaveId) {
  char expectedHeader[5];
  snprintf(expectedHeader, sizeof(expectedHeader), "D%dTD", slaveId);
  return strncmp(data, expectedHeader, 4) == 0;
}