#include "Arduino.h"
#include "Buzzer.h"
#include <ArduinoSTL.h>

#include "morse.h"
#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include "messages.h"
#include "LcdDisplay.h"
#include "utils.h"

//I2C pins declaration
LiquidCrystal_I2C liqLcd(0x3F, 2, 1, 0, 4, 5, 6, 7, 3, POSITIVE);
LcdDisplay lcd(liqLcd);

using std::string;

#define NOTE_C7 2093
Buzzer buzzer;
const int morsePin = 2;
const int correctPin = 3;
const int buzzerPin = 8;
const int BUTTON_DELAY = 300;
const std::string allCities[] = { "COSTA RICA", "SOUTH AFRICA", "AUSTRALIA",
		"BANGKOK", "DUBAI", "VANCOUVER", "AINSA", "SAN FRANCISCO" };
const std::string cities[] = { "BANGKOK", "DUBAI" };

uint8_t cityGuessIndex = 0;

string morseSequence;
string cityName;
int idleCnt = 0;
int errorCnt = 0;

enum ProgState {
	INIT, CITIES, DONE
};

enum ErrorState {
	OK, WRONG_CITY, WRONG_MORSE
};

ErrorState errorState = ErrorState::OK;
ProgState state = ProgState::INIT;

void lcdPrint(const uint8_t row, std::string msg) {
	lcd.println(row, msg);
}

char handleHigh() {
	int cnt = 0;
	while (digitalRead(morsePin) == HIGH) {
		digitalWrite(buzzerPin, HIGH);
		cnt++;
		if (cnt > 20) {
			Serial.println("- pressed");
			digitalWrite(buzzerPin, LOW);
			delay(BUTTON_DELAY);
			return '-';
		}
		delay(10);
	}

	Serial.println(". pressed");
	digitalWrite(buzzerPin, LOW);
	delay(BUTTON_DELAY);
	return '.';
}

void playMorseChar(const std::string morseChar) {
	for (uint8_t i = 0; i < morseChar; i++) {
		const char sChar = morseChar.at(i);
		if (sChar == '-') {
			buzzer.singleTone(4000, 500);
		} else {
			buzzer.singleTone(4000, 100);
		}
		delay(1000);
	}
}

void playMorseString(const std::string aString) {
	for (uint8_t i = 0; i < aString.size(); i++) {
		std::string morseSequence = encodeMorseCharacter(aString.at(i));
		playMorseChar(morseSequence);
	}
}

bool isInCities(const std::string value) {

	for (uint8_t i = 0; i < ArraySize(allCities); i++) {
		if (value.compare(allCities[i]) == 0) {
			return true;
		}
	}
	return false;
}

bool checkCorrectCity(const std::string cityName) {
	return cities[cityGuessIndex].compare(cityName) == 0;
}

void handleCorrectionButton() {

	buzzer.singleTone(NOTE_C7, 100);
	if (morseSequence.length() > 0) {
		morseSequence.pop_back();
		Serial.print("Morse sequence corrected: ");
		Serial.println(morseSequence.c_str());
	} else if (cityName.length() > 0) {
		cityName.pop_back();
		Serial.print("Code name corrected: ");
		Serial.println(cityName.c_str());
	}
	delay(BUTTON_DELAY);

}

//The setup function is called once at startup of the sketch
void setup() {
	buzzer.attach(buzzerPin);
	pinMode(morsePin, INPUT);
	pinMode(correctPin, INPUT);

	// initialize lcd
	Serial.begin(9600);
	lcd.init();
	lcdPrint(0, "Let's start");
	buzzer.singleTone(2000, 200);
	state = ProgState::CITIES;
}

void resetIdleTime() {
	idleCnt = 0;
}

const char* getStateString() {
	switch (state) {
	case INIT:
		return getMessage(MessageCmd::MSG_INIT);
	case CITIES:
		switch (cityGuessIndex) {
		case 0:
			return getMessage(MessageCmd::MSG_FIRST_CITY);
		case 1:
			return getMessage(MessageCmd::MSG_SECOND_CITY);
		default:
			return getMessage(MessageCmd::MSG_THIRD_CITY);
		}
		break;
	case DONE:
		return getMessage(MessageCmd::MSG_DONE);
	}
	return "";
}

bool checkState() {
	if (errorState != ErrorState::OK) {
		lcdPrint(0, "Fout!");
		delay(5000);
		errorState = ErrorState::OK;
		//Serial.println("Error state");
		return true;
	}

	if (state == ProgState::DONE) {
		lcdPrint(0, getMessage(MessageCmd::MSG_DONE));
		lcdPrint(1, "2367");
		//Serial.println("Done");
		return false;
	}

	if (cityName.length() == 0 && morseSequence.length() == 0) {
		const char* msg = getStateString();
		lcdPrint(0, msg);
		//Serial.println((String)"State: " + msg);
		return true;
	}

	// print cityname and morse sequence in case no of the other states applies
	//Serial.println((String)"Print city and morse: " + cityName.c_str() + ", " + morseSequence.c_str());
	lcdPrint(0, cityName);
	lcdPrint(1, morseSequence);

	return true;
}

// The loop function is called in an endless loop
void loop() {

	//Add your repeated code here
	if (checkState()) {
		if (digitalRead(morsePin) == HIGH) {
			idleCnt = 0;
			morseSequence += handleHigh();
			Serial.println((String)"MorseSequence: " + morseSequence.c_str());
			//lcdPrint(1, morseSequence.c_str());
		} else if (digitalRead(correctPin) == HIGH) {
			idleCnt = 0;
			handleCorrectionButton();
		} else {
			// no button touched, we are in idle time
			idleCnt++;
			if (idleCnt > 100) {
				// check whether there is something to decode, reset the counter in either way
				idleCnt = 0;
				if (morseSequence.length() > 0) {
					std::string character = decodeMorseCharacter(morseSequence);

					if (character.length() > 0) {
						Serial.println((String)"Character found: " + character.c_str());
						morseSequence.clear();
						cityName += character;
						Serial.println((String)"City name: " + cityName.c_str());

						if (isInCities(cityName)) {
							Serial.println((String)"City found: " + cityName.c_str());
							if (checkCorrectCity(cityName)) {
								if(cityGuessIndex++ >= (ArraySize(cities) -1)) {
									state = ProgState::DONE;
								}
							} else {
								errorState = ErrorState::WRONG_CITY;
							}
							cityName.clear();
						}
					}
				}
			}
			delay(10);
		}
	}
}
