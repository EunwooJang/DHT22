#include "DHT_edit.h"
#include <SoftwareSerial.h>

// HC-12 핀 설정 (RX, TX)
SoftwareSerial HC12(10, 11); // HC-12 모듈 연결 핀 (TX, RX)

// DHT 핀 설정
#define DHTPIN1 2
#define DHTPIN2 3
#define DHTPIN3 4
#define DHTPIN4 5
#define DHTPIN5 6

// DHT 객체 생성
DHT22Raw dht1;
DHT22Raw dht2;
DHT22Raw dht3;
DHT22Raw dht4;
DHT22Raw dht5;

// 고정된 헤더를 저장할 배열
char fixedHeaderData[5] = "D1TD"; // 고정 헤더

// 이전 측정 데이터를 저장할 변수
char lastSentData[25]; // 4바이트 헤더 + 20바이트 센서 데이터

void setup() {
  HC12.begin(9600); // HC-12 시리얼 통신 시작

  // 이전 전송 데이터 초기화
  memset(lastSentData, 0, sizeof(lastSentData));

  // DHT 센서 초기화
  dht1.reset();
  dht2.reset();
  dht3.reset();
  dht4.reset();
  dht5.reset();
}

void loop() {
  // HC-12로부터 명령 수신 대기
  if (HC12.available() >= 4) {
    char command[5];
    HC12.readBytes(command, 4);
    command[4] = '\0';

    // 데이터 요구 명령어 처리 (S1TD)
    if (strcmp(command, "S1TD") == 0) {
      sendSensorData();
    }
    // 이전 데이터 재전송 명령어 처리 (S1TU)
    else if (strcmp(command, "S1TU") == 0) {
      resendLastData();
    }
  }
}

// 센서 데이터를 읽고 전송
void sendSensorData() {
  char rawData[20];
  memset(rawData, 0, sizeof(rawData));

  // 각 DHT 센서에서 데이터를 읽음
  readSensorData(dht1, DHTPIN1, &rawData[0]);
  readSensorData(dht2, DHTPIN2, &rawData[4]);
  readSensorData(dht3, DHTPIN3, &rawData[8]);
  readSensorData(dht4, DHTPIN4, &rawData[12]);
  readSensorData(dht5, DHTPIN5, &rawData[16]);

  // "D1TD" 헤더 전송
  memcpy(lastSentData, fixedHeaderData, 4); // 헤더 저장
  memcpy(&lastSentData[4], rawData, 20);    // 센서 데이터 저장

  // HC-12로 데이터 전송
  HC12.write(lastSentData, sizeof(lastSentData));
}

// 이전 데이터를 재전송
void resendLastData() {
  HC12.write(lastSentData, sizeof(lastSentData));
}

// 센서 데이터를 읽음
void readSensorData(DHT22Raw &dht, uint8_t pin, char *buffer) {
  uint32_t rawData = dht.read(pin);

  if (rawData == DHT22RAW_INVALID_VALUE) {
    rawData = 0; // 유효하지 않은 값일 경우 0으로 설정
  }

  // 센서 데이터를 4바이트로 복사
  memcpy(buffer, &rawData, 4);
}