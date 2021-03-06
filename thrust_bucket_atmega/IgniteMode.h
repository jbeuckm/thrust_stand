/*
 * IgniteMode.h
 *
 *  Created on: Feb 11, 2017
 *      Author: joebeuckman
 */

#ifndef IGNITEMODE_H_
#define IGNITEMODE_H_

#include <SD.h>
#include "BaseMode.h"

#define IGNITER_PIN A0

class IgniteMode: public BaseMode {
private:
	unsigned long timestamp = 0;
	unsigned long lastMillis = 0;
	const int chipSelect = 8;
	String thrustDataFilename;
	File thrustDataFile;
	void setupSDcard();
	void finish();
	String findNextFilename();

	void writeDirectoryCsv();

public:
	IgniteMode(HX711 *_scale, rgb_lcd *_lcd);
	void startMode();
	int updateMode();
};

#endif /* IGNITEMODE_H_ */
