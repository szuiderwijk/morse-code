/*
 * LcdDisplay.h
 *
 *  Created on: 24 Dec 2018
 *      Author: stephanzuiderwijk
 */

#ifndef LCDDISPLAY_H_
#define LCDDISPLAY_H_

#include <ArduinoSTL.h>
#include <LiquidCrystal_I2C.h>


class LcdDisplay {
private:
	LiquidCrystal_I2C lcd;
	std::string rows[2];

public:
	LcdDisplay(const LiquidCrystal_I2C& lcd);
	virtual ~LcdDisplay();
	void println(const uint8_t row, const std::string msg);
	void init();
};

#endif /* LCDDISPLAY_H_ */
