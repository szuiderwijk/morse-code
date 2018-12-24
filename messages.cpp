/*
 * messages.h
 *
 *  Created on: 15 Dec 2018
 *      Author: stephanzuiderwijk
 */

#ifndef MESSAGES_CPP_
#define MESSAGES_CPP_

#include <avr/pgmspace.h>
#include "messages.h"
#include "progmem.h"

// number of items in an array
template<typename T, size_t N> size_t ArraySize(T (&)[N]) {
	return N;
}

const char* getMessage(MessageCmd code) {

	MsgCode thisItem;
	PROGMEM_readAnything(&messages[code], thisItem);

	return thisItem.name; //As cmd.name resolves to a pointer it can be passed back as is.
	//However to use the 'pointed to data' it will have to be accessed properly.

	return 0;
}

#endif /* MESSAGES_CPP_ */
