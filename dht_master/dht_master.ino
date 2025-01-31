#include "DHT_multi.h"
#include <SoftwareSerial.h>

// 슬레이브 개수 설정
#define TEMP_SLAVE_AMOUNT 1

SoftwareSerial hc12(D2, D1); // HC-12 모듈을 위한 SoftwareSerial 객체

// DHTMulti 객체 생성
DHTMulti dhtMulti(TEMP_SLAVE_AMOUNT);

void setup() {
  Serial.begin(115200);
  hc12.begin(9600); // HC-12 통신 시작 (메인 함수에서 설정)
}

void loop() {
  char* data = dhtMulti.getAllSensorData();

  // 수집된 데이터를 출력
  for (int i = 0; i < SLAVE_AMOUNT; i++) {
    Serial.print("Slave ");
    Serial.print(i + 1);
    Serial.println(": ");
    
    for (int j = 0; j < 20; j += 2) { // 2바이트씩 묶음
      uint16_t value = (data[j + 1] << 8) | data[j];  // Little Endian 변환
      float divided_value = value / 10.0;  // 10으로 나누기
      
      // 변환된 십진수 출력
      Serial.print(divided_value, 1); // 소수점 1자리 출력
      Serial.print(" ");
    }
    
    Serial.println();
  }

  delay(3000); // 3초 대기 후 다음 데이터 요청
}
