#include <SoftwareSerial.h>

const int ERROR       = 2;  // ERROR
const int NORMAL      = 4;  // NORMAL 
const int ALARM       = 3;  // ALARM
const int CONNECTED   = 13;  // CONNECTED

int pinA = 7; // -> PD13
int pinB = 8; // -> PD14 STM32

SoftwareSerial mySerial(10, 11); // RX, TX

void setup() {
  Serial.begin(9600);       // UART chính (D0/D1, Serial Monitor)
  mySerial.begin(9600);     // UART phụ (SoftwareSerial)

  pinMode(ERROR    , OUTPUT);
  pinMode(NORMAL   , OUTPUT);
  pinMode(ALARM    , OUTPUT);
  pinMode(CONNECTED, OUTPUT);

  pinMode(pinA, INPUT);
  pinMode(pinB, INPUT);

  digitalWrite(ERROR    , 0);
  digitalWrite(NORMAL   , 1);
  digitalWrite(ALARM    , 0);
  digitalWrite(CONNECTED, 1);

  Serial.println("Ready.");
}

void loop() {
  if (mySerial.available()) {
    Serial.print("From SoftwareSerial: ");
    Serial.println(mySerial.read());
  }


  int analogValue = analogRead(A0);  // Đọc giá trị analog từ chân A0 (0-1023)
  
  // In ra giá trị lên Serial Monitor
  Serial.print("Analog Value: ");
  Serial.println(analogValue);
  //Serial.println(b);
  delay(200); // 1ms sampling

  //  // Bật LED 1, tắt LED 2
  // digitalWrite(ledPin1, HIGH);
  // digitalWrite(ledPin13, HIGH);
  // digitalWrite(ledPin2, LOW);
  // delay(500); // Delay 500ms

  // // Tắt LED 1, bật LED 2
  // digitalWrite(ledPin1, LOW);
  // digitalWrite(ledPin2, HIGH);
  // delay(500); // Delay 500ms
  
}
