#include <HX711_ADC.h>
#if defined(ESP8266)|| defined(ESP32) || defined(AVR)
#include <EEPROM.h>
#endif
const int HX711_dout = 12;
const int HX711_sck = 13;
HX711_ADC LoadCell(HX711_dout, HX711_sck);
const int calVal_eepromAdress = 0;
unsigned long t = 0;

#include <Stepper.h>
const int stepsPerRevolution = 200;
Stepper myStepper(stepsPerRevolution, 8, 9, 10, 11);

#include <Wire.h>
#include <LiquidCrystal_I2C.h>
int x = 0;
int count = 0;
int state = 0;
LiquidCrystal_I2C lcd(0x27, 16, 2); // set the LCD address to 0x27 for a 16 chars and 2 line display

void setup()
{
  Serial.begin(9600);
  lcd.begin();
  lcd.setCursor(0,0);
  lcd.print("Starting...");
  lcd.setCursor(0,1);
  lcd.print("Setup All..");
  myStepper.setSpeed(100);
  LoadCell.begin();
  float calibrationValue;
  calibrationValue = -379.5;
  #if defined(ESP8266)|| defined(ESP32)
  #endif
  
  unsigned long stabilizingtime = 2000;
  boolean _tare = true;
  LoadCell.start(stabilizingtime, _tare);
  if (LoadCell.getTareTimeoutFlag()) {
    Serial.println("Timeout, check MCU>HX711 wiring and pin designations");
    while (1);
  }
  else {
    LoadCell.setCalFactor(calibrationValue);
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("Startup is complete");
  }

  // Print a message to the LCD.
  lcd.begin();
  lcd.backlight();
  lcd.setCursor(0, 0);
  lcd.print("Jum Biji= ");
  Serial.println("Jum Biji= ");
  Serial.print(x);
  lcd.print(x);
  lcd.setCursor(0,1);
  lcd.print("Berat (g)= ");
//  lcd.print(i,7);
}
void loop()
{
  myStepper.step(stepsPerRevolution);
  int counter = digitalRead(A0);
  if (state == 0) {
    switch (counter) {
      case 1 : state = 1; lcd.setCursor (0, 1); x = x + 1; lcd.print(x); Serial.print(x);break;
      case 0 : state = 0; break;
    }
  }
  if (counter == LOW) {
    state = 0;
  }
  static boolean newDataReady = 0;
  const int serialPrintInterval = 0;
  if (LoadCell.update()) newDataReady = true;
  if (newDataReady) {
    if (millis() > t + serialPrintInterval) {
      double i = LoadCell.getData();
      newDataReady = 0;
      t = millis();
      lcd.clear();
      if(i<=0.1) {
        i=0.0;    
      } 
      if(i<=5000) {      
        lcd.clear();
        lcd.setCursor(0, 0);
        lcd.print("Jum Biji= ");
        lcd.print(x);
        Serial.print(x);
        lcd.setCursor(0,1);
        lcd.print("Berat (g)= ");
        lcd.print(i,7);
      }
    }
  }
  if (Serial.available() > 0) {
    char inByte = Serial.read();
    if (inByte == 't') LoadCell.tareNoDelay();
  }
  // check if last tare operation is complete:
  if (LoadCell.getTareStatus() == true) {
    Serial.println("Tare complete");
  }
}