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
const std::string allCities[] = { "COSTA RICA", "SOUTH AFRICA", "AUSTRALIA", "BANGKOK", "DUBAI", "VANCOUVER", "AINSA",
        "SAN FRANCISCO" };
const std::string cities[] = { "BANGKOK", "DUBAI" };

uint8_t cityGuessIndex = 0;

string morseSequence;
string cityName;
string currentDisplayMessage;
int idleCnt = 0;
int errorCnt = 0;

enum ConnectionState {
    SENDING, RECEIVING
};

enum ProgState {
    INIT, CITIES, DONE
};

enum ErrorState {
    OK, WRONG_CITY, WRONG_MORSE
};

ErrorState errorState = ErrorState::OK;
ProgState progState = ProgState::INIT;
ConnectionState connState = ConnectionState::RECEIVING;
bool answeringStarted = false;

void lcdPrint(const uint8_t row, std::string msg) {
    lcd.println(row, msg);
}

char handleHigh() {
    int cnt = 0;
    while (digitalRead(morsePin) == HIGH) {
        digitalWrite(buzzerPin, HIGH);
        cnt++;
        delay(10);
    }
    digitalWrite(buzzerPin, LOW);
    if (cnt > 20) {
        Serial.println("- pressed");
        return '-';
    } else {
        Serial.println(". pressed");
        return '.';
    }
}

void playMorseChar(const std::string morseChar) {
    string displaySequence;
    for (uint8_t i = 0; i < morseChar.size(); i++) {
        const char sChar = morseChar.at(i);
        displaySequence += sChar;
        if (sChar == '-') {
            buzzer.singleTone(3000, 400);
        } else {
            buzzer.singleTone(3300, 100);
        }
        lcdPrint(1, displaySequence);
        delay(400);
    }
}

void playMorseString(const std::string aString) {
    string displayMessage;
    for (uint8_t i = 0; i < aString.size(); i++) {
        displayMessage += aString.at(i);
        std::string morseSequence = encodeMorseCharacter(aString.at(i));
       // Serial.println((String) "Encoded morse sequence: " + morseSequence.c_str());
        playMorseChar(morseSequence);
        lcdPrint(0, displayMessage);
        delay(500);
    }
    lcdPrint(1, "");
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
//        Serial.print("Morse sequence corrected: ");
//        Serial.println(morseSequence.c_str());
    } else if (cityName.length() > 0) {
        cityName.pop_back();
//        Serial.print("Code name corrected: ");
//        Serial.println(cityName.c_str());
    }
    delay(BUTTON_DELAY);

}

//The setup function is called once at startup of the sketch
void setup() {
    buzzer.attach(buzzerPin);
    pinMode(morsePin, INPUT);
    pinMode(correctPin, INPUT);

    Serial.begin(9600);
    lcd.init();
    buzzer.singleTone(2000, 200);
    progState = ProgState::CITIES;
}

void resetIdleTime() {
    idleCnt = 0;
}

const char* getStateString() {
    switch (progState) {
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
        playMorseString("Fout!");
        delay(5000);
        errorState = ErrorState::OK;
        // Serial.println("Error state");
        return true;
    }

    if (progState == ProgState::DONE) {
        playMorseString(getMessage(MessageCmd::MSG_DONE));
        delay(3000);
        playMorseString("2367");
        return false;
    }

    if (cityName.length() == 0 && morseSequence.length() == 0 && connState == ConnectionState::RECEIVING) {
        string msg = getStateString();
            currentDisplayMessage = msg;
            playMorseString(msg);
            // switch to sending so user can use morse-key
            answeringStarted = false;
            connState = ConnectionState::SENDING;
            return true;
    }

    // print cityname and morse sequence in case no of the other states applies
    if (answeringStarted) {
        lcdPrint(0, cityName);
        lcdPrint(1, morseSequence);
    }

    return true;
}

// The loop function is called in an endless loop
void loop() {

    //Add your repeated code here
    if (checkState() && connState == ConnectionState::SENDING) {
        if (digitalRead(morsePin) == HIGH) {
            answeringStarted = true;
            idleCnt = 0;
            morseSequence += handleHigh();
            Serial.println((String) "MorseSequence: " + morseSequence.c_str());
            delay(BUTTON_DELAY);
            //lcdPrint(1, morseSequence.c_str());
        } else if (digitalRead(correctPin) == HIGH) {
            idleCnt = 0;
            handleCorrectionButton();
        } else {
            // no button touched, we are in idle time
            idleCnt++;
            if (idleCnt > 80) {
                // check whether there is something to decode, reset the counter in either way
                idleCnt = 0;
                if (morseSequence.length() > 0) {
                    std::string character = decodeMorseCharacter(morseSequence);

                    if (character.length() > 0) {
                        Serial.println((String) "Character found: " + character.c_str());
                        morseSequence.clear();
                        cityName += character;
                        Serial.println((String) "City name: " + cityName.c_str());

                        if (isInCities(cityName)) {
                            Serial.println((String) "City found: " + cityName.c_str());
                            if (checkCorrectCity(cityName)) {
                                if (cityGuessIndex++ >= (ArraySize(cities) - 1)) {
                                    progState = ProgState::DONE;
                                }
                            } else {
                                errorState = ErrorState::WRONG_CITY;
                            }
                            cityName.clear();
                            // switch back to receiving state for new instructions
                            connState = ConnectionState::RECEIVING;
                        }
                    }
                }
            }
            delay(10);
        }
    }
}
