#include <WiFi.h>
#include <WiFiClient.h>
#include <BlynkSimpleEsp32.h>
#include <ESP32Servo.h>
#define BLYNK_PRINT Serial  
#include <TimeLib.h>
#include <WidgetRTC.h>
#include "HX711.h"

#define DOUT  16 
#define CLK  17 
HX711 scale(DOUT, CLK);
float calibration_factor = 10670; //15765 12670
int GRAM;

const char* ssid = "MiA2";       //wifi name
const char* password = "simalakama";       //password
char auth[] = "6PcE1jdBBVSPHmOmSXQyCMuhhbCGhjcP";          //Auth Code sent by Blynk

int servoPin = 2;
Servo Myservo;
BlynkTimer timer;
WidgetRTC rtc;

unsigned long startMillis;
unsigned long currentMillis;
const unsigned long period= 1000;

void setup(){
  Serial.begin(115200);
  scale.set_scale();
  scale.tare();//reset scale ke 0
  long zero_factor = scale.read_average(); //baseline reading
  setSyncInterval(1);
  Blynk.begin(auth, ssid, password);
  while (Blynk.connect()==false){}
  setSyncInterval(10*60); // Sync interval in seconds (10 minutes)
  timer.setInterval(10000L, clockDisplay);
  startMillis = millis();
  Myservo.attach(servoPin);
}

void loop()
{
  Blynk.run();
  scale.set_scale(calibration_factor); //adjust calibration factor
  GRAM = scale.get_units(5);
  Blynk.virtualWrite(V7, GRAM);
  timer.run();
}

void clockDisplay()
{
  currentMillis = millis();
  if(currentMillis - startMillis > period){
  String currentTime = String(hour()) + ":" + minute() + ":" + second();
  String currentDate = String(day()) + " " + month() + " " + year();
  Serial.print("Current time: ");
  Serial.print(currentTime);
  Serial.print(" ");
  Serial.print(currentDate);
  Serial.println();
  Blynk.virtualWrite(V4, currentTime);
  // Send date to the App
  Blynk.virtualWrite(V5, currentDate);
  }
}  

BLYNK_CONNECTED() {
  // Synchronize time on connection
  rtc.begin();
}

BLYNK_WRITE(V2){
  Myservo.write(0);
  delay(15);
}

BLYNK_WRITE(V3){
  Myservo.write(180);
    delay(15);

}

//cadanganSLider
BLYNK_WRITE(V6){
  Myservo.write(param.asInt());
}
