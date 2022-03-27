#include <LiquidCrystal_I2C.h>
#include <Wire.h>

//const int rs = 12, en = 11, d4 = 5, d5 = 4, d6 = 3, d7 = 2;
LiquidCrystal_I2C lcd(0x27,16,2);
char buffer[256];

void setup() {
  
  lcd.init();
  lcd.backlight();
  
  lcd.setCursor(0,0);
  lcd.print("Welcome to LA4");
  lcd.setCursor(0,0);
  
  Serial.begin(115200);
  delay(2000);
}

void ClearDisplay() {

  lcd.setCursor(0,0);
  lcd.print("                "); 
  lcd.setCursor(0,1);
  lcd.print("                "); 
  
}

void loop() {
  if(Serial.available() > 0){
    //text = Serial.readStringUntil('\n');
    
    Serial.readBytes(buffer, 256);
    lcd.setCursor(0,0);
    lcd.print("                "); 
    lcd.setCursor(0,0);
    lcd.print(buffer);
    Serial.print("Text updated!\n");
  }
}
