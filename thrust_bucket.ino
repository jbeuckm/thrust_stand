/*
  This example code uses bogde's excellent library: https://github.com/bogde/HX711
*/
#include <avr/pgmspace.h>

#include <SPI.h>
#include <SD.h>


#include <Wire.h>
#include "rgb_lcd.h"
rgb_lcd lcd;


#include "HX711.h"
#define HX711_DOUT  7
#define HX711_CLK  6 
HX711 scale(HX711_DOUT, HX711_CLK);


#include "CalibrationMode.h"
CalibrationMode calibrationMode(scale, lcd);


#include "RotaryEncoder.h"
RotaryEncoder wheel(4);

void buttonDown() {

}
void buttonUp() {

}

volatile int wheelRotation = 0;

void rotateWheel(int direction) {
  wheelRotation += direction;
}

const int chipSelect = 8;
File thrustDataFile;

void setupSDcard() {
  Serial.println(F("Initting SD..."));
  pinMode(SS, OUTPUT);

  if (!SD.begin(chipSelect)) {
    Serial.println(F("SD init failed"));
  }
  Serial.println(F("SD initted"));

  String thrustFilename = F("thrust.tsv");

  //  SdFile::dateTimeCallback(dateTime);
  thrustDataFile = SD.open(thrustFilename, O_WRITE | O_CREAT | O_TRUNC);

  if (!thrustDataFile) {
    Serial.print(F("error opening "));
    Serial.println(thrustFilename);
  }

  thrustDataFile.println(F("millis\tload"));
}




#define SPEAKER_PIN 6
#define IGNITER_PIN A0
#define BUTTON_PIN A1


enum MISSION_STATE {
  CALIBRATION,
  COUNTDOWN,
  FIRE,
  COOLDOWN
};


MISSION_STATE mission_state = CALIBRATION;

void setupRotaryEncoder() {
  attachInterrupt(0, interruptA, RISING); // set an interrupt on PinA, looking for a rising edge signal and executing the "PinA" Interrupt Service Routine (below)
  attachInterrupt(1, interruptB, RISING); // set an interrupt on PinB, looking for a rising edge signal and executing the "PinB" Interrupt Service Routine (below)
  wheel.setHandleButtonDown(buttonDown);
  wheel.setHandleButtonUp(buttonUp);
  wheel.setHandleRotate(rotateWheel);
}

void interruptA() {
  wheel.PinA();
}
void interruptB() {
  wheel.PinB();
}


void setup() {

  Serial.begin(9600);
/*  
  Serial.println("HX711 calibration sketch");
  Serial.println("Remove all weight from scale");
  Serial.println("After readings begin, place known weight on scale");
  Serial.println("Press + or a to increase calibration factor");
  Serial.println("Press - or z to decrease calibration factor");
*/

  scale.set_scale();
  scale.tare(); //Reset the scale to 0


  long zero_factor = scale.read_average(); //Get a baseline reading
//  Serial.print("Zero factor: "); //This can be used to remove the need to tare the scale. Useful in permanent scale projects.
//  Serial.println(zero_factor);

  lcd.begin(16, 2);

  pinMode(SPEAKER_PIN, OUTPUT);
  pinMode(IGNITER_PIN, OUTPUT);

  setupRotaryEncoder();
  wheel.checkButton();
 
  begin_calibration();
//  begin_firing();
//  start_countdown();

}



void begin_calibration() {

  calibrationMode.startMode();

  mission_state = CALIBRATION;
}

void loop() {

  wheel.checkButton();
 
  switch (mission_state) {
    
    case CALIBRATION:
      if (wheelRotation != 0) {
        calibrationMode.handleWheelRotation(wheelRotation);
        wheelRotation = 0;
      }
      calibrationMode.updateMode();
      break;

    case COUNTDOWN:
      countdown_loop();
      break;
    
    case FIRE:
      fire_loop();
      break;
  }
}




unsigned long last_millis = 0;



unsigned long timestamp;
int frequency;

void start_countdown() {
  lcd.setRGB(255, 0, 0);
  lcd.print(F("COUNTDOWN"));

  timestamp = millis();
  mission_state = COUNTDOWN;
}

void countdown_loop() {
  frequency = (millis() - timestamp) % 1000;
  tone(SPEAKER_PIN, 150 + frequency);

  
  lcd.setCursor(0,1);
  lcd.print(150 + frequency);

  if ( (millis() - timestamp) >= 10000) {
    noTone(SPEAKER_PIN);
    begin_firing();
  }

  if (digitalRead(BUTTON_PIN) == LOW) {
    begin_calibration();
  }
}

void begin_firing() {
  lcd.clear();
  lcd.setRGB(255, 0, 0);
  lcd.print(F("FIRING!!!"));

  timestamp = millis();

  fire_loop();
  digitalWrite(IGNITER_PIN, HIGH);
  mission_state = FIRE;
}

void fire_loop() {
  unsigned long next_millis = millis();

  scale.read();

//  thrustDataFile.print(next_millis - timestamp);
//  thrustDataFile.print("\t");
//  thrustDataFile.println(scale.get_units());

  if ( (next_millis - timestamp) >= 5000) {
    finish_firing();
  }
}


void finish_firing() {

  mission_state = COOLDOWN;
  lcd.clear();
  lcd.setRGB(0, 0, 255);
  lcd.print(F("COOLDOWN"));

  digitalWrite(IGNITER_PIN, LOW);
  fire_loop();

  thrustDataFile.flush();
  thrustDataFile.close();

}

