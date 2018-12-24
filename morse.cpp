#include "morse.h"
#include <ArduinoSTL.h>
#include "progmem.h"

// number of items in an array
template<typename T, size_t N> size_t ArraySize(T (&)[N]) {
	return N;
}


std::string decodeMorseCharacter(std::string sequence) {
	std::string result = "";
	for (uint8_t i = 0; i < ArraySize(map_table); i++) {
		MorseMap thisItem;
		PROGMEM_readAnything(&map_table[i], thisItem);

		if (strcmp(sequence.c_str(), thisItem.morse) == 0) {
			result = thisItem.token;
			break;
		}
	}
	return result;
}
