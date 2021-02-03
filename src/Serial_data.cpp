#include "Serial_data.h"
#include "MemoryFree.h"
#include "deserializeJson.hpp"
char *incoming_data_from_gsm = "";
String outgoing_data = "";
const byte numChars = 500;
byte receivedChars[numChars];
boolean newData = false;
void updateSerial1()
{

    static boolean recvInProgress = false;
    static byte ndx = 0;
    char startMarker = '{';
    char endMarker = '}';
    char rc;
    while (Serial1.available() != 0 && newData == false)
    {
        
        rc = Serial1.read();

        if (recvInProgress == true)
        {
            if (rc != endMarker)
            {
                receivedChars[ndx] = rc;
                ndx++;
                if (ndx >= numChars)
                {
                    ndx = numChars - 1;
                }
            }
            else
            {
                receivedChars[ndx] = '\0'; // terminate the string
                recvInProgress = false;
                ndx = 0;
                newData = true;
                // Serial.print("received char is: ");
                // Serial.println(receivedChars);
            }
        }

        else if (rc == startMarker)
        {
            recvInProgress = true;
        }
    }
    if (newData == true)
    {
        // Serial.print("char array data is: ");
        // Serial.println(receivedChars);

        // strncat(incoming_data_from_gsm,"{",1);
        incoming_data_from_gsm = strdup((char *)receivedChars);
        strncat(incoming_data_from_gsm, "}", 1);
        // Serial.print("Received data after concatenation is: ");
        // Serial.println(incoming_data_from_gsm);

        outgoing_data = "{";
        outgoing_data += incoming_data_from_gsm;
        // Serial.print("Outgoing data after concatenation is: ");
        // Serial.println(outgoing_data);

        memset(receivedChars, 0, sizeof(receivedChars));
        memset(incoming_data_from_gsm, 0, sizeof(incoming_data_from_gsm));

        outgoing_data.trim();
        incoming_data_from_gsm = strdup(outgoing_data.c_str());
        // Serial.print("Received data after concatenation is: ");
        Serial.println(incoming_data_from_gsm);
        outgoing_data = "";
        deserialization(incoming_data_from_gsm);
        newData = false;
    }
}