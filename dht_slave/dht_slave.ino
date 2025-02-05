#include "DHT_edit.h"

#define SLAVE_ID 1
#define SENSOR_TYPE 'T'

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

char command1[5];
char command2[5];
  
// 고정된 헤더를 저장할 배열
char fixedHeaderData[5]; // 동적으로 설정되는 헤더

// 이전 측정 데이터를 저장할 변수
char lastSentData[24]; // 4바이트 헤더 + 20바이트 센서 데이터

void setup() {
  Serial1.begin(9600); // HC-12 시리얼 통신 시작

	// 명령어 문자열 생성
  snprintf(command1, sizeof(command1), "SA%cD", SENSOR_TYPE);
  snprintf(command2, sizeof(command2), "S%d%cD", SLAVE_ID, SENSOR_TYPE);

  // 동적으로 헤더 초기화
  snprintf(fixedHeaderData, sizeof(fixedHeaderData), "D%dTD", SLAVE_ID);

  // 이전 전송 데이터 초기화
  memset(lastSentData, 0, sizeof(lastSentData));

   pinMode(10, OUTPUT);
}

void loop() {

  if (Serial1.available() > 0) {  // 데이터 수신 시작 감지

    unsigned long startTime = millis(); // 시작 시간 저장
  
    // 최대 10ms 동안 기다리면서 4바이트 도착 여부 확인
    while ((millis() - startTime) < 10) {
  
      if (Serial1.available() >= 4) {
  
    			char receivedCommand[4];  // 수신 데이터 저장
    			Serial1.readBytes(receivedCommand, 4);
    			
          // 수신된 명령어와 미리 정의된 명령어 비교
    			if (memcmp(receivedCommand, command1, 4) == 0) {
       		  digitalWrite(10, HIGH);
            
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
            
    			} else if (memcmp(receivedCommand, command2, 4) == 0) {
            
            sendLastData();
    			
          }

          digitalWrite(10, LOW);

				return;
			}
    }

    // 10ms가 지나도 4바이트가 안 모이면 버퍼 초기화
    while (Serial1.available()) {
      Serial1.read();
    }
  }
}

// 이전 데이터를 재전송
void sendLastData() {
  Serial1.write(lastSentData, sizeof(lastSentData));
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
