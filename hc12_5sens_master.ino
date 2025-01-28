//CODE by Minseo Kim

void setup() {
  Serial.begin(9600);    // USB 시리얼 통신
  Serial1.begin(9600);   // HC-12 통신
  Serial.println("HC-12 Master 준비 완료");
}

void loop() {
  // 사용자 입력을 받아 HC-12로 전송
  if (Serial.available() > 0) {
    String command = Serial.readStringUntil('\n'); // 사용자 입력 읽기
    command.trim(); 

    if (command == "start") {
      Serial1.println(command); // HC-12로 명령 전송
      Serial.println("전송: " + command); // 디버깅 메시지
    }
  }

  // Slave로부터 데이터 수신
  if (Serial1.available() > 0) {
    String received = Serial1.readStringUntil('\n'); // 데이터 수신
    Serial.println("수신: " + received); // 수신 데이터 출력

    // 데이터 파싱 및 출력
    if (received.startsWith("DATA:")) {
      received = received.substring(5); // "DATA:" 제거
      String sensors[5];
      int index = 0;

      // 각 센서 데이터를 분리
      while (received.indexOf(";") != -1) {
        sensors[index++] = received.substring(0, received.indexOf(";"));
        received = received.substring(received.indexOf(";") + 1);
      }
      sensors[index] = received;

      // 센서 데이터 출력
      Serial.println("=== 센서 데이터 ===");
      for (int i = 0; i < 5; i++) {
        int commaIndex = sensors[i].indexOf(',');
        String temperature = sensors[i].substring(0, commaIndex);
        String humidity = sensors[i].substring(commaIndex + 1);
        Serial.print("센서 ");
        Serial.print(i + 1);
        Serial.print(" | 온도: ");
        Serial.print(temperature);
        Serial.print(" °C | 습도: ");
        Serial.print(humidity);
        Serial.println(" %");
      }
      Serial.println("===================");
    } else if (received == "DATA:ERROR") {
      Serial.println("센서 데이터를 읽을 수 없습니다!");
    }
  }
}
