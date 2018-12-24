/*
 * messages.h
 *
 *  Created on: 15 Dec 2018
 *      Author: stephanzuiderwijk
 */

#ifndef MESSAGES_H_
#define MESSAGES_H_


#include <avr/pgmspace.h>

struct MsgCode {
  int code;
  const char* name;
};

enum MessageCmd {
  MSG_INIT,
  MSG_FIRST_CITY,
  MSG_SECOND_CITY,
  MSG_THIRD_CITY,
  MSG_INCORRECT_MORSE,
  MSG_INCORRECT_CITY,
  MSG_WRONG_CITY,
  MSG_DONE
};

const MsgCode messages[] PROGMEM = {
  { MSG_INIT, "Initialiseren" } ,
  { MSG_FIRST_CITY, "1ste stad?"} ,
  { MSG_SECOND_CITY, "2de stad?"},
  { MSG_THIRD_CITY, "3de stad?"},
  { MSG_INCORRECT_MORSE, "Morse fout!"},
  { MSG_INCORRECT_CITY, "Onbekende stad!"},
  { MSG_WRONG_CITY, "Verkeerde stad!"},
  { MSG_DONE, "OK, De code is:"}
};

#define FIRST_COMMAND 0
#define LAST_COMMAND sizeof(messages) / sizeof(*messages)

const char* getMessage(MessageCmd code);


#endif /* MESSAGES_CPP_ */
