#include "DHT_multi.h"
#include <SoftwareSerial.h>

// HC-12 핀 설정 (RX, TX)
SoftwareSerial HC12(10, 11); // HC-12 모듈 연결 핀 (TX, RX)

// 슬레이브 개수 설정
#define SLAVE_AMOUNT 5

// DHTMulti 객체 생성
DHTMulti dhtMulti(SLAVE_AMOUNT);

void setup() {
  Serial.begin(115200);
  HC12.begin(9600); // HC-12 통신 시작

  dhtMulti.begin(); // DHTMulti 초기화
}

void loop() {
  char* data = dhtMulti.getAllSensorData();

  // 수집된 데이터를 출력
  for (int i = 0; i < SLAVE_AMOUNT; i++) {
    Serial.print("Slave ");
    Serial.print(i + 1);
    Serial.print(": ");
    for (int j = 0; j < 20; j++) {
      Serial.print(data[i * 20 + j], HEX);
      Serial.print(" ");
    }
    Serial.println();
  }

  delay(3000); // 3초 대기 후 다음 데이터 요청
}
