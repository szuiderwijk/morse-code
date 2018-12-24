/*
 * utils.h
 *
 *  Created on: 24 Dec 2018
 *      Author: stephanzuiderwijk
 */

#ifndef UTILS_H_
#define UTILS_H_

#include <ArduinoSTL.h>

// number of items in an array
template<typename T, size_t N> size_t ArraySize(T (&)[N]) {
	return N;
}


#endif /* UTILS_H_ */
