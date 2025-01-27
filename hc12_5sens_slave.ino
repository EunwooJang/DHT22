#include <DHT.h>

// DHT22 핀 및 타입 정의
#define DHTPIN1 20
#define DHTPIN2 19
#define DHTPIN3 18
#define DHTPIN4 15
#define DHTPIN5 14
#define DHTTYPE DHT22

// DHT 객체 생성
DHT dht1(DHTPIN1, DHTTYPE);
DHT dht2(DHTPIN2, DHTTYPE);
DHT dht3(DHTPIN3, DHTTYPE);
DHT dht4(DHTPIN4, DHTTYPE);
DHT dht5(DHTPIN5, DHTTYPE);

void setup() {
  Serial1.begin(9600);  // HC-12 통신 초기화
  dht1.begin();
  dht2.begin();
  dht3.begin();
  dht4.begin();
  dht5.begin();
}

void loop() {
  // HC-12로부터 명령 수신
  if (Serial1.available() > 0) {
    String command = Serial1.readStringUntil('\n'); // 명령 수신
    command.trim(); // 공백 제거

    if (command == "start") {
      // 각 센서 데이터 읽기
      float t1 = dht1.readTemperature();
      float h1 = dht1.readHumidity();
      float t2 = dht2.readTemperature();
      float h2 = dht2.readHumidity();
      float t3 = dht3.readTemperature();
      float h3 = dht3.readHumidity();
      float t4 = dht4.readTemperature();
      float h4 = dht4.readHumidity();
      float t5 = dht5.readTemperature();
      float h5 = dht5.readHumidity();

      // 오류 처리
      if (isnan(t1) || isnan(h1) || isnan(t2) || isnan(h2) || isnan(t3) || isnan(h3) || 
          isnan(t4) || isnan(h4) || isnan(t5) || isnan(h5)) {
        Serial1.println("DATA:ERROR"); // 오류 메시지 전송
      } else {
        // 데이터 포맷팅 및 전송
        String data = "DATA:" + String(t1) + "," + String(h1) + ";" +
                      String(t2) + "," + String(h2) + ";" +
                      String(t3) + "," + String(h3) + ";" +
                      String(t4) + "," + String(h4) + ";" +
                      String(t5) + "," + String(h5);
        Serial1.println(data);
      }
    }
  }
}
