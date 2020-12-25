/*==========SETTINGS==========*/
#define VOLT_OFFSET     2
#define VOLT_MULTIPLIER 0.0161
#define AMP_OFFSET      761
#define AMP_MULTIPLIER  0.001

/*=============LIBRARIES=============*/
#include <TimeLib.h>
#include "buildTime.h"//parses PCs time

#include <Wire.h>
#include <LiquidCrystal_I2C.h>
LiquidCrystal_I2C lcd(0x27, 20, 4);

#include <SD.h>
const int chipSelect = BUILTIN_SDCARD;

/*======PINS======*/
#define VOLT_PIN 15
#define AMP_PIN  14
#define RST_PIN  1
#define SDA_PIN  18
#define SCL_PIN  19

/*======VARIABLES======*/
float volts;
float amps;
float watts;
float wattsAvg;
float wattsSum;
float wattHours;
float totalWattHours;
float prevWattHours;
float totalKWhHours;

byte currentSecond, prevSecond;
unsigned long currentTime, prevTime;
byte prevMinute, prevHour;

void setup() {
  /*===========RESET===========*/
  pinMode(RST_PIN, INPUT_PULLUP);

  /*===========LCD===========*/
  //Wire.begin();
  Wire.setSDA(SDA_PIN);
  Wire.setSCL(SCL_PIN);
  
  lcd.init();
  lcd.backlight();

  lcd.setCursor(4, 1);
  lcd.print(F("SOLAR LOGGER"));

  delay(2000);
  lcd.clear();

  /*=============================TIME & DATE SET=============================*/
  setTime(BUILD_HOUR, BUILD_MIN, BUILD_SEC, BUILD_DAY, BUILD_MONTH, BUILD_YEAR);
}

void loop() {

  /*=====TIME & DATE=====*/
  currentSecond = second();
  currentTime = millis();

  /*============================MEASUREMENTS===========================*/
  volts = (float)(analogRead(VOLT_PIN) - VOLT_OFFSET) * VOLT_MULTIPLIER; // (Raw volt value - offset) * multiplier
  //volts = (float)analogRead(VOLT_PIN); // Read raw volt value
  amps = (float)(analogRead(AMP_PIN) - AMP_OFFSET) * AMP_MULTIPLIER;     // (Raw amp value - offset) * multiplier
  //amps = (float)analogRead(VOLT_PIN);  // Read raw amp value
  watts = (float)volts * amps;

  if (amps < 0) {
    watts = 0.0;
  }

  if ((currentTime - prevTime) < 0) {
    watts = 0.0;
  }

  /*======================================CALCULATIONS======================================*/
  wattHours = (float)wattHours + watts * ((currentTime - prevTime) * 0.000000277);
  totalWattHours = (float)(totalWattHours + watts * ((currentTime - prevTime) * 0.000000277));
  totalKWhHours = (float)totalWattHours * 0.001;

  prevTime = currentTime;

  if (currentTime > 4294967000) {
    prevTime = 0;
  }

  /*=============RESET=============*/
  if (digitalRead(RST_PIN) == LOW) {
    wattHours = 0.0;
  }

  /*==============LCD==============*/
  if (currentSecond != prevSecond) {
    
    // First line
    lcd.setCursor(1, 0); // (column, line)
    lcd.print(volts, 2);
    lcd.setCursor(5, 0);
    lcd.print(F("V "));
    lcd.setCursor(8, 0);
    lcd.print(amps, 2);
    lcd.setCursor(12, 0);
    lcd.print(F("A "));

    lcd.setCursor(15, 0);
    if (watts > 10 && watts < 100) {
      lcd.print(F(" "));
    }
    else if (watts < 10) {
      lcd.print(F("  "));
    }
    lcd.print(watts, 0);
    lcd.setCursor(18, 0);
    lcd.print(F("W "));

    // Second line
    lcd.setCursor(3, 1);
    lcd.print(F("Energy "));
    lcd.setCursor(11, 1);
    lcd.print(wattHours);
    lcd.setCursor(14, 1);
    lcd.print(F("Wh"));

    // Third line
    lcd.setCursor(3, 2);
    lcd.print(F("Total"));
    lcd.setCursor(10, 2);
    lcd.print(totalKWhHours, 2);
    lcd.setCursor(13, 2);
    lcd.print(F("kWh"));

    // Fourth line
    lcd.setCursor(1, 3);
    if (hour() < 10) {
      lcd.print(F("0"));
    }
    lcd.print(hour());

    lcd.print(F(":"));

    lcd.setCursor(4, 3);
    if (minute() < 10) {
      lcd.print(F("0"));
    }
    lcd.print(minute());
    
    lcd.setCursor(9, 3);
    if (day() < 10) {
      lcd.print(F("0"));
    }
    lcd.print(day());

    lcd.setCursor(11, 3);
    lcd.print(F("/"));
    
    lcd.setCursor(12, 3);
    if (month() < 10) {
      lcd.print(F("0"));
    }
    lcd.print(month());
 
    lcd.setCursor(14, 3);
    lcd.print(F("/"));
    lcd.setCursor(15, 3);
    lcd.print(year());

    prevSecond = currentSecond;
  }
  
  /*==============SD==============*/
  if (hour() != prevHour) {

    wattsAvg = totalWattHours - prevWattHours;
    prevWattHours = totalWattHours;

    SD.begin(chipSelect);

    String dataString = "";
    dataString += String(hour());
    dataString += F(":");
    dataString += String(minute());
    dataString += F("; ");
    
    dataString += String(day());
    dataString += F("/");
    dataString += String(month());
    dataString += F("/");
    dataString += String(year());
    dataString += F("; ");
    
    dataString += String(volts);
    dataString += F(", ");
    dataString += String(wattsAvg);
    dataString += F(", ");
    dataString += String(totalWattHours);

    File dataFile = SD.open("SOLARLOG.txt", FILE_WRITE);

    prevMinute = minute();
    prevHour = hour();
  }

  delay(20);
}
