#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>
#include "DHT.h"
#include <string.h>

#define DHTPIN   13 // DHT11 센서 핀
#define DHTTYPE  DHT11 // DHT 센서 타입 (DHT11, DHT21, DHT22 중 DHT11)

DHT dht(DHTPIN, DHTTYPE);

char *ssid = "SSID"; // 와이파이 SSID
char *password = "PW"; // 와이파이 비밀번호 (없을 경우 ""로)

const char CLIENT_REFRESH_INTERVAL_SECOND[] = "2"; // 초 단위로 클라이언트 측 새로고침 주기 설정

ESP8266WebServer server(80); // 웹서버 포트 80으로 설정
String message = ""; // 메시지 전역변수 설정
String base = ""; // 기본 메시지 전역변수 설정 (전체 html 시작 <head> 부분이랑 <body> 부분 앞쪽)
String foot = ""; // 기본 메시지 전역변수 설정 (전체 html 끝 <body> 부분 뒷쪽)
String root_code = ""; // 전체 서버 코드 (오류 미발생시)

float DI_calc(float t, float h){ // 불쾌지수 계산 함수
  float DI = 0.81*t+0.01*h*(0.99*t-14.3)+46.3; // 불쾌지수 계산식
  return DI;
}

void handleRoot(){
  float h = dht.readHumidity(); // DHT11 센서 습도 값
  float t = dht.readTemperature(); // DHT11 센서 온도 값
  float DI; // 불쾌지수
  server.sendHeader("Refresh", CLIENT_REFRESH_INTERVAL_SECOND); // 클라이언트 새로고침 주기 명령

  int light;
  message = ""; // 메시지 초기화
  // MoodCircle 시작
  message += "&nbsp;&nbsp;<div id=\"MoodCircle\" style=\"width: 600px; height: 600px; border-radius: 50%; margin-left: 160px; margin-right: 160px; margin-top:50px; margin-bottom:50px; background: radial-gradient(circle at center, ";
  if(t>=28){
    message += "#ff0000";
  }
  else if(t>=30){
    message += "#ff4000";
  }
  else if(t>=26){
    message += "#ff8000";
  }
  else if(t>=22){
    message += "#ffbf00";
  }
  else if(t>=18){
    message += "#ffff00";
  }
  else if(t>=8){
    message += "#bfff00";
  }
  else if(t>=4){
    message += "#80ff00";
  }
  else if(t>=0){
    message += "#40ff00";
  }
  else{
    message += "#00ff00";
  }
  message += ", ";
  if(h>=80){
    message += "#0000ff";
  }
  else if(h>=60){
    message += "#0000bf";
  }
  else if(h>=40){
    message += "#000080";
  }
  else if(h>=20){
    message += "#000040";
  }
  else{
    message += "#000000";
  }
  message += ")\" align=\"center\">";
  message += "<div style=\"display: flex; justify-content: center; align-items: center; flex-direction: column; border-radius: 50%;\">";
  
  // emoji 시작
  message += "<div style=\"font-size: 200px; margin-top: 50px; margin-bottom: 50px;\">";
  // set DI font size to 100px
  DI = DI_calc(t, h);
  if(isnan(DI)){
    message += "🤔";
  }
  else if(DI>=85){
    message += "⛔️";
  }
  else if(DI>=80){
    message += "⚠️";
  }
  else if(DI>=75){
    message += "🥵";
  }
  else if(DI>=70){
    message += "😰";
  }
  else if(DI>=65){
    message += "👍";
  }
  else if(DI>=60){
    message += "😊";
  }
  else if(DI>=55){
    message += "🥶";
  }
  else if(DI>=50){
    message += "⚠️";
  }
  else if(DI<50){
    message += "⛔️";
  }
  else{
    message += "🤔";
  }
  message += "<br><p style=\"font-size: 100px; margin-top: 30px; margin-bottom: 50px; color: white; text-shadow: 2px 2px 0px #FF0000; font-weight: 550\"><strong>";
  if(isnan(DI)){ // DI 계산 불가시 / 센서 오류시
    message += "오류";
  }
  else{
    message += DI;
  }
  message += "</strong></p>";
  message += "</div>";
  // emoji 끝
  message += "</div></div>";
  message += "<br>";
  // MoodCircle 끝
  
  // 온습도 시작
  message += "<p style=\"font-size: 30px\">&nbsp;&nbsp;현재 온도는 🌡️&nbsp;<strong><font color = \"red\">";
  message += t;
  message += "&deg;C</font></strong>, 습도는 💧<strong><font color = \"blue\">";
  message += h;
  message += "%</font></strong>입니다.</p>";
  message += "<p style=\"font-size: 30px\">&nbsp;&nbsp;";
  if(t>=28){
    message += "☀️ 날씨가 덥네요. 얇은 옷을 입고 나가는게 어떨까요?<br>&nbsp;&nbsp;너무 오랜 외출은 위험할 수 있어요!";
  }
  else if(t>=33){
    message += "🥵 오늘 날씨가 너무 더운데, 외출은 자제하시는 게 어떨까요?";
  }
  else if(t>=20){
    message += "👕 적당한 온도네요. 편한 차림으로 나가셔도 될 것 같아요!";
  }
  else if(t>=10){
    message += "🍂 오늘은 좀 쌀쌀하네요. 겉옷을 잘 챙겨입으세요!<br>&nbsp;&nbsp;너무 오랜 외출은 위험할 수 있어요!";
  }
  else if(t<10){
    message += "🥶 오늘 날씨가 너무 추운데, 외출은 자제하시는 게 어떨까요?";
  }
  message += "<br>";
  message += "&nbsp;&nbsp;&nbsp;";
  if(h>=60){
    message += "💦 오늘은 <strong>꽤 습하네요.</strong> 땀이 많이 날 수 있으니,<br>&nbsp;&nbsp; <strong>과격한 운동은 하지 않는 게 좋을 것</strong> 같아요.";
  }
  else if(h<=40){
    message += "🏜️ 오늘은 <strong>너무 건조</strong>하네요.<br>&nbsp;&nbsp; <strong>물을 많이 마시는 게 어떨까요?</strong> 로션을 바르는 것도 좋을 것 같아요!";
  }
  else if(h>40 && h<60){
    message += "👍 습도가 적당하네요. <font color=\"green\">쾌적한 날씨</font>에요!";
  }
  else{
    message += "⛔️ <strong><font color = \"red\">온습도 감지에 문제가 발생했습니다. 연결을 확인해 주세요.</font></strong>";
  }
  message += "</p>";
  // 온습도 끝
  // 조도 시작
  light=analogRead(A0); // A0핀 조도센서
  if(light<=100){
    message="<script>var audio = new Audio('https://github.com/hoony6134/publichost/raw/main/beep-warning-6387.mp3');audio.play();setTimeout(() => console.log(\"warning!\"), 3000);alert(\'혹시 넘어지셨나요? 저조도의 환경이 감지되었습니다.\')</script>"; // 넘어짐 감지: 경고음 & 알림창
  }
  else{
    message += "&nbsp;&nbsp;<br>";
    message+="<font size = \"30px\"><strong>💡 현재 조도: "+String(light)+"</strong></font><p style=\"font-size: 30px\">";
    message += "&nbsp;&nbsp;";
    if(light<=300){
      message+="🌙 넘어짐이 감지되지 않았지만,<br>&nbsp;&nbsp;주변이 조금 어두운 것 같아요.<br>&nbsp;&nbsp;밝은 곳으로 이동하는 건 어떨까요?";
    }
    else if(light<=500){
      message+="🌤 주변이 적당히 밝은 것 같아요! 좋아요!";
    }
    else{
      message+="🌞 주변이 꽤 밝은 것 같아요.<br>&nbsp;&nbsp;눈 건강을 위해 아주 조금만<br>&nbsp;&nbsp;어두운 곳으로 가도 될 것 같아요!";
    }
    message+="</p>";
  }
  // 조도 끝

  // HTML 구조 작성
  base = "<!DOCTYPE html><head><title>Elder Guardian</title><style>@import url('https://fonts.googleapis.com/css2?family=Nanum+Gothic&display=swap');html * {font-family: 'Nanum Gothic', sans-serif;}</style></head><body><div>&nbsp;&nbsp;<font size = \"35\"><strong>🛡️ Elder Guardian</strong></font></div>&nbsp;&nbsp;<font size = \"28\">Developed by JeongHoon Lim with ❤️</font>";
  // 긴급신고버튼 시작
  foot = "<button style=\"font-size: 70px;\" onclick=\"window.open('tel:119');\"><strong>🚑 긴급전화 (119)</strong></button>";
  // 긴급신고버튼 끝
  foot += "</body></html>"; // HTML 끝
  root_code = base + message + foot;
  server.send(200, "text/html; charset=utf-8", root_code); // Status Code 200: OK로 전체 코드 전송
}

void setup(void) {
  Serial.begin(115200); // 시리얼 통신 시작 (baud rate: 115200)
  dht.begin(); // 온습도 센서 시작
  WiFi.begin(ssid, password); // WiFi 연결 시작

  while (WiFi.status() != WL_CONNECTED){
    delay(500);
    Serial.print(".");
  } // WiFi 연결 대기
  
  server.on("/", handleRoot); // 루트 페이지에 대한 요청이 들어오면 handleRoot 함수 실행
  server.begin(); // 서버 시작
}

void loop() {
  server.handleClient(); // 클라이언트 요청 처리
}