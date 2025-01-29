#include "DHT_multi.h"
#include <Arduino.h>

// 외부에서 선언된 HC12 참조
extern SoftwareSerial HC12;

DHTMulti::DHTMulti(uint8_t slaveAmount)
  : slaveAmount(slaveAmount) {
  combinedData = new char[20 * slaveAmount];
  memset(combinedData, 0, 20 * slaveAmount);
}

void DHTMulti::begin() {
  // 필요시 추가적인 초기화 코드
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
      if (!requestResendData(i, buffer)) {
        Serial.print("Slave ");
        Serial.print(i);
        Serial.println(" failed again.");
      }
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

	if (HC12.available()) {
 		unsigned long startTime = millis();
  	while ((millis() - startTime) < 200) { // Wait up to 100ms for response
    	if (HC12.available() >= 24) {
      	HC12.readBytes(buffer, 24);
      	return validateReceivedData(buffer, slaveId);
    	}
  	}
		
		while (HC12.available()) {
			HC12.read();
		}
	}

  return false; // Timed out

}

bool DHTMulti::requestResendData(uint8_t slaveId, char* buffer) {
  char command[5];
  snprintf(command, 5, "S%dTU", slaveId);

  HC12.write(command);

	if (HC12.available()) {
 		unsigned long startTime = millis();
  	while ((millis() - startTime) < 200) { // Wait up to 100ms for response
    	if (HC12.available() >= 24) {
      	HC12.readBytes(buffer, 24);
      	return validateReceivedData(buffer, slaveId);
    	}
  	}
		
		while (HC12.available()) {
			HC12.read();
		}
	}

  return false; // Timed out

}

bool DHTMulti::validateReceivedData(const char* data, uint8_t slaveId) {
  char expectedHeader[5];
  snprintf(expectedHeader, sizeof(expectedHeader), "D%dTD", slaveId);

  if (strncmp(data, expectedHeader, 4) != 0) {
    return false; // Header mismatch
  }

  return true;
}
