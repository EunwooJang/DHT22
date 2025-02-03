#include "DHT_multi.h"
#include <SoftwareSerial.h>

// 슬레이브 개수 설정
#define TEMP_SLAVE_AMOUNT 5

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
  // 모든 데이터를 바이너리 형태로 한 줄로 출력
  for (int i = 0; i < TEMP_SLAVE_AMOUNT * 20; i += 2) {  
      uint16_t value = (uint16_t)data[i] | ((uint16_t)data[i + 1] << 8); // 리틀 엔디언 변환
      Serial.print(value, DEC); // 십진수(DEC) 출력
      Serial.print(" ");  // 값 사이에 공백 추가
  }
  Serial.println();  // 줄 바꿈
  delay(3000); // 3초 대기 후 다음 데이터 요청
}
