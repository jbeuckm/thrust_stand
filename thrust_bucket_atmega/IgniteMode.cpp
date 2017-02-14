/*
 * IgniteMode.cpp
 *
 *  Created on: Feb 11, 2017
 *      Author: joebeuckman
 */

#include "IgniteMode.h"
#include "ChangeModeFunction.h"


IgniteMode::IgniteMode(HX711 *_scale, rgb_lcd *_lcd): BaseMode(_scale, _lcd) {
	scale = _scale;
	lcd = _lcd;
	functionIndex = 0;

	FUNCTION_COUNT = 1;

	modeFunctions = (BaseModeFunction **) malloc(FUNCTION_COUNT * sizeof(BaseModeFunction*));

	modeFunctions[0] = new ChangeModeFunction(F("[Cancel]"), -2);

	setupSDcard();

	Serial.println(F("IgniteMode() complete"));
}


void IgniteMode::setupSDcard() {
	pinMode(IGNITER_PIN, OUTPUT);

	pinMode(SS, OUTPUT);

	if (!SD.begin(chipSelect)) {
		Serial.println(F("SD init failed"));
	}

	String thrustFilename = F("thrust.tsv");

	//  SdFile::dateTimeCallback(dateTime);
	thrustDataFile = SD.open(thrustFilename, O_WRITE | O_CREAT | O_TRUNC);

	if (!thrustDataFile) {
		Serial.print(F("error opening "));
		Serial.println(thrustFilename);
	}

	thrustDataFile.println(F("millis\traw\tload"));
}




void IgniteMode::startMode() {

	lcd->clear();
	lcd->setRGB(255, 200, 0);
	lcd->print(F("FIRING!!!"));

	BaseMode::startMode();

	timestamp = millis();
	updateMode();
	digitalWrite(IGNITER_PIN, HIGH);
	timestamp = millis();
}


int IgniteMode::updateMode() {

  unsigned long next_millis = millis();
  int elapsed = next_millis - timestamp;

  double value = scale->get_value(1);

  thrustDataFile.print(elapsed);
  thrustDataFile.print("\t");
  thrustDataFile.print(value);
  thrustDataFile.print("\t");
  thrustDataFile.println(value / scale->get_scale());

  if ( elapsed >= 5000) {
    finish();
    return 1;
  }

  return 0;
}


void IgniteMode::finish() {

  digitalWrite(IGNITER_PIN, LOW);

  thrustDataFile.flush();
  thrustDataFile.close();
}
