#include <SoftwareSerial.h>

SoftwareSerial bluetooth(4, 5);  // 5번 핀: 블루투스 RX, 4번 핀: 블루투스 TX.

const int dirPin = 2;
const int stepPin = 3;
const int enablePin = 8;   // ENABLE 핀 (모터 드라이버 활성화/비활성화)

String lastInput = "";  // 마지막 입력 명령 저장 변수

void setup() {
  pinMode(stepPin, OUTPUT);
  pinMode(dirPin, OUTPUT);
  pinMode(enablePin, OUTPUT);  // ENABLE 핀 출력 모드 설정
  
  // 모터 초기 상태 설정 (모터 정지)
  digitalWrite(enablePin, HIGH);  // 초기 상태: 모터 드라이버 비활성화
  digitalWrite(stepPin, LOW);
  digitalWrite(dirPin, LOW);

  bluetooth.begin(9600);   // 블루투스 시리얼 시작
}

void loop() {
  if (bluetooth.available() > 0) {
    String input = bluetooth.readStringUntil('\n');
    input.trim();  // 공백 제거

    // 중복된 입력 방지
    if (input == lastInput) {
      bluetooth.println("중복된 명령입니다. 무시합니다.");
      return;  // 중복된 명령은 처리하지 않음
    }

    lastInput = input;  // 현재 입력을 저장

    int commaIndex = input.indexOf(',');
    if (commaIndex > 0) {
      String direction = input.substring(0, commaIndex);
      int steps = input.substring(commaIndex + 1).toInt();

      if (steps > 0) {
        digitalWrite(enablePin, LOW);  // 모터 드라이버 활성화
        rotateMotor(steps, direction);  
        bluetooth.println("모터가 " + String(steps) + " 스텝 회전했습니다.");
        digitalWrite(enablePin, HIGH);  // 모터 드라이버 비활성화
      } else {
        bluetooth.println("유효하지 않은 스텝 수입니다.");
      }
    } else {
      bluetooth.println("유효하지 않은 입력 형식입니다.");  // 쉼표가 없을 경우 오류 메시지
    }
    
    bluetooth.flush();
    delay(2000); //2초 후에 다시 응답 받을 준비.
  }
}

// 모터 회전 함수
void rotateMotor(int steps, String direction) {
  digitalWrite(dirPin, direction == "CW" ? HIGH : LOW); // CW면 HIGH, CCW면 LOW

  for (int i = 0; i < steps; i++) { //지정된 스텝 수만큼 반복
    digitalWrite(stepPin, HIGH);
    delayMicroseconds(1000);  // 펄스 유지 시간
    digitalWrite(stepPin, LOW);
    delayMicroseconds(1000);  // 펄스 간 간격
  }
  delay(2000);  // 모터 회전 후 2초 대기, rotateMotor() 종료 -> loop()로 다시 돌아감, 블루투스 명령 대기.
}