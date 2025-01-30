#include "DHT_edit.h"

// HC-12 핀 설정: Serial 포트를 HC-12에 직접 연결
#define HC12 Serial

// DHT 핀 설정
#define DHTPIN1 20
#define DHTPIN2 19
#define DHTPIN3 18
#define DHTPIN4 14
#define DHTPIN5 15

// DHT 객체 생성
DHT22Raw dht1;
DHT22Raw dht2;
DHT22Raw dht3;
DHT22Raw dht4;
DHT22Raw dht5;

// 슬레이브 ID 설정
const uint8_t SLAVE_ID = 1; // 이 값은 반드시 변경 가능해야 함

// 고정된 헤더를 저장할 배열
char fixedHeaderData[5]; // 동적으로 설정되는 헤더

// 이전 측정 데이터를 저장할 변수
char lastSentData[25]; // 4바이트 헤더 + 20바이트 센서 데이터

void setup() {
  HC12.begin(9600); // HC-12 시리얼 통신 시작

  // 동적으로 헤더 초기화
  snprintf(fixedHeaderData, sizeof(fixedHeaderData), "D%dTD", SLAVE_ID);

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
  if (HC12.available() > 0) {  // 데이터 수신 시작 감지
    unsigned long startTime = millis(); // 시작 시간 저장

    // 최대 10ms 동안 기다리면서 4바이트 도착 여부 확인
    while ((millis() - startTime) < 10) {
      if (HC12.available() == 4) {  // 4바이트가 도착하면 처리
        char command[4];
        HC12.readBytes(command, 4);

        if (command[0] == 'S' && command[1] == ('0' + SLAVE_ID)) {
      			if (command[2] == 'M' && command[3] =='D') {    
						sendSensorData();
    				}	else if (command[2] == 'M' && command[3] =='U') {
						resendLastData();
					}   
				}

        return; // 정상 처리 후 루프 종료
      }
    }

    // 10ms가 지나도 4바이트가 안 모이면 버퍼 초기화
    while (HC12.available()) {
      HC12.read();
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

  // 동적으로 설정된 헤더 전송
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
