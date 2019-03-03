/*
 * LcdDisplay.cpp
 *
 *  Created on: 24 Dec 2018
 *      Author: stephanzuiderwijk
 */

#include "LcdDisplay.h"

LcdDisplay::LcdDisplay(const LiquidCrystal_I2C& inLcd) :
        lcd(inLcd) {
    // TODO Auto-generated constructor stub

}

LcdDisplay::~LcdDisplay() {
    // TODO Auto-generated destructor stub
}

void LcdDisplay::println(const uint8_t row, const std::string msg) {
    if (rows[row].compare(msg) != 0) {
        rows[row] = msg;
        lcd.clear();
//	    Serial.println((String)"Adjusting lcd.");
        for (short i = 0; i < 2; i++) {
            std::string displayString = rows[i];
            if (rows[i].length() > 16) {
                displayString = displayString.substr((uint8_t)(displayString.length() - 16), 16);
            }
            lcd.setCursor(0, i);
            lcd.print(displayString.c_str());
        }
    } else {
        //Serial.println((String)"Msg: " + msg + " equals: " + rows[row]);
    }
}

//void LcdDisplay::printScrolling(const uint8_t row, const std::string msg) {
//    if (rows[row].compare(msg) != 0) {
//            rows[row] = msg;
//            lcd.clear();
//    //      Serial.println((String)"Adjusting lcd.");
//            for (short i = 0; i < 2; i++) {
//                lcd.setCursor(0, i);
//                lcd.print(rows[i].c_str());
//                for (short j = 0; j < (rows[i].length() - 16); j++) {
//                    lcd.scrollDisplayLeft();
//                }
//            }
//        } else {
//            //Serial.println((String)"Msg: " + msg + " equals: " + rows[row]);
//        }
//}

void LcdDisplay::init() {
    lcd.begin(16, 2); //Defining 16 columns and 2 rows of lcd display
    lcd.backlight(); //To Power ON the back light
}
