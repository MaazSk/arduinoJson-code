#ifndef	_GSMDECLARATIONS_H
#define _GSMDECLARATIONS_H

#include <Arduino.h>
#include <ArduinoJson.h>

int8_t sendATcommand(String ATcommand, char *expected_answer1, char *expected_answer2, unsigned int timeout);    //tO send AT commands

void MQTTconfigure();    //To configure MQTT 

boolean connectgprs();  //To connect to gprs

boolean GsmConnectModule(); //basic gsm commands

void GSM_INIT();    //GSM initialization function consist all above function

#endif

