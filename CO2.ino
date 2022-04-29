#include "Arduino_SensorKit.h"
#include <CCS811.h>

int button = 4;
int buzzer = 5;
int LED = 6;
int ledOn = 0;
float co2max = 150.00;
float co2 = 00.00;
float tvoc = 00.00;
int pot_value;
int trigger = 0;
unsigned long timeLedOn;
unsigned long timeTrigger;

#define POT A0

CCS811 sensor;

void setup() {
  Oled.begin();
  Oled.setFlipMode(true);

  Serial.begin(115200);

  pinMode(button, INPUT);
  pinMode(buzzer, OUTPUT);
  pinMode(LED, OUTPUT);

  digitalWrite(LED, LOW);
  digitalWrite(buzzer, LOW);

  while(sensor.begin() != 0){
    Serial.println("failed to init chip, please check if the chip connection is fine");
    delay(1000);
  }
  
  sensor.setMeasCycle(sensor.eCycle_250ms);
}

void loop() {
  if(digitalRead(button)){
    trigger = 1;
    timeTrigger = millis();
  }
  if(millis() > timeTrigger + 10000){
    trigger = 0;
  }
  if(sensor.checkDataReady() == true){
    if(!trigger){
      co2 = sensor.getCO2PPM();   
      tvoc = sensor.getTVOCPPB();   
    } else {
      co2 = 999.99;
      tvoc = 999.99;
    }
  } else {
    co2 = 9999.99;
    tvoc = 9999.99;
    Serial.println("Data is not ready!");
  }
  
  Oled.setFont(u8x8_font_chroma48medium8_r); 
  Oled.setCursor(1, 2);
  Oled.print("CO2:  ");
  Oled.print(co2);
  Oled.print("  ");
  Oled.setCursor(1, 4);
  Oled.print("TVOC: ");
  Oled.print(tvoc);
  Oled.print("  ");

  pot_value = analogRead(POT);

  if(co2 >= co2max){
    if(!ledOn){
      digitalWrite(LED, HIGH);
      timeLedOn = millis();
      ledOn = 1;
    }
  } else {
    digitalWrite(LED, LOW);
    ledOn = 0;
  }

  if(ledOn){
    if(millis() > timeLedOn + 5000){
      tone(buzzer, pot_value);
    }
  } else {
    noTone(buzzer);
  }

  sensor.writeBaseLine(0x847B);
  
  delay(1000);
}
