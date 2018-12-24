/*
 * morse.h
 *
 *  Created on: 9 Dec 2018
 *      Author: stephanzuiderwijk
 */

#ifndef MORSE_CPP_
#define MORSE_CPP_


#include <avr/pgmspace.h>
#include <ArduinoSTL.h>

struct MorseMap {
	const char* morse;
	char token;
};

static MorseMap const map_table[] PROGMEM = {
		{".-", 'A'},
		{"-...", 'B'},
		{"-.-.", 'C'},
		{"-..", 'D'},
		{".", 'E'},
		{"..-.", 'F'},
		{"--.", 'G'},
		{"....", 'H'},
		{"..", 'I'},
		{".---", 'J'},
		{"-.-", 'K'},
		{".-..", 'L'},
		{"--", 'M'},
		{"-.", 'N'},
		{"---", 'O'},
		{".--.", 'P'},
		{"--.-", 'Q'},
		{".-.", 'R'},
		{"...", 'S'},
		{"-", 'T'},
        {"..-", 'U'},
        {"...-", 'V'},
        {".--", 'W'},
        {"-..-", 'X'},
        {"-.--", 'Y'},
        {"--..", 'Z'},
		{"---..", ' '}
};
static const int MORSE_TABLE_LENGTH = 26;

std::string decodeMorseCharacter(std::string sequence);


#endif /* MORSE_CPP_ */
