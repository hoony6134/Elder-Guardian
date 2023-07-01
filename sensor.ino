#include <LiquidCrystal_I2C.h>
#include <Wire.h>

// 포트 설정
int trigPin = 13;
int echoPin = 12;
int speakerPin = 8;
int numTones = 8;
int tones[]= {392, 349, 392, 349, 392, 349, 392, 349}; // 삐용삐용삐용삐용

// 신호등 LED 핀
const int red = 5;
const int yellow = 6;
const int green = 7;

const int interval = 200; // 새로고침 주기 (ms)

LiquidCrystal_I2C lcd(0x27,16,2);

void setup(){
  Serial.begin(115200);
  pinMode(5, OUTPUT);
  pinMode(6, OUTPUT);
  pinMode(7, OUTPUT);
  pinMode(trigPin, OUTPUT);
  pinMode(echoPin, INPUT);
  lcd.init();
  lcd.backlight();
  lcd.setCursor(0,0);
  lcd.print("EG Approtector"); // Approach + Protector (EG: Elder Guardian - 노약자 보호자)
  lcd.setCursor(0,1);
  lcd.print("Dist: ");
  lcd.setCursor(10,1);
  lcd.print("cm");
}

void loop(){
  long duration, distance;
  digitalWrite(trigPin, LOW);
  delayMicroseconds(2);
  digitalWrite(trigPin, HIGH);
  delayMicroseconds(10);
  digitalWrite(trigPin, LOW);
  duration = pulseIn(echoPin, HIGH); // 초음파가 돌아오는 시간 측정
  distance = duration * 17 / 1000; // 시간을 거리 (cm 단위)로 변환
  if(distance>=1000) distance = 0;
  if (distance<=10){ // 거리가 10cm 이내일 때 (누군가 접근하는 것으로 감지 - 적색 LED + 별도 적색 LED 경고등 + 삐용삐용 부저)
    digitalWrite(red, HIGH);
    digitalWrite(yellow, LOW);
    digitalWrite(green, LOW);
    lcd.setCursor(15,0);
    lcd.print("!");
    for(int i = 0; i < numTones; i++) // 삐용삐용 부저
    {
      digitalWrite(red, HIGH);
      tone(speakerPin, tones[i]);
      delay(250);
      digitalWrite(red, LOW);
      delay(250);
    }
    noTone(speakerPin);
    delay(100);
    lcd.setCursor(15,0);
    lcd.print(" ");
  }
  else if (distance>10 && distance<=20){ // 거리가 10cm 초과 20cm 이하일 때 (주의 - 황색 LED)
    digitalWrite(red, LOW);
    digitalWrite(yellow, HIGH);
    digitalWrite(green, LOW);
  }
  else{
    digitalWrite(red, LOW);
    digitalWrite(yellow, LOW);
    digitalWrite(green, HIGH);
  }
  Serial.println(distance); // 거리를 시리얼 모니터에 출력
  lcd.setCursor(6,1); // 거리를 LCD에 출력
  lcd.print("    "); // 4자리 숫자를 출력하기 위해 먼저 공백 4칸을 출력 (초기화)
  lcd.setCursor(6,1); // 다시 커서를 6번째 칸으로 이동
  lcd.print(distance);
  delay(interval);
}