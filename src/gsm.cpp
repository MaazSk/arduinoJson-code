#include "gsmdeclarations.hpp"

unsigned int resp;
unsigned int period = 20000; //20sec
unsigned long lastSend = 0;
String stationID = "STN-2";
String publishDat = ""; //json data to be published is stored in this string

int8_t sendATcommand(String ATcommand, char *expected_answer1, char *expected_answer2, unsigned int timeout)
{
    uint8_t x = 0, answer = 0;
    char response[300];
    unsigned long previous;

    memset(response, 0, 100); // Initialize the string

    delay(100);

    while (Serial1.available() > 0)
        Serial1.read(); // Clean the input buffer

    Serial1.println(ATcommand); // Send the AT command
                                //    delay(500);
                                //    while(Serial1.available())         //from server data coming
                                //   {
                                //      String inData =Serial1.readString();
                                //      Serial.println(inData);
                                //
                                //      inData.toCharArray(response,300);
                                //
                                //      if (strstr(response, expected_answer1) != NULL)
                                //      {
                                //                answer = 1;
                                //       }
                                //            // check if the desired answer 2 is in the response of the module
                                //       else if (strstr(response, expected_answer2) != NULL)
                                //      {
                                //                answer = 2;
                                //      }
                                //   }
                                //  return answer;
                                // }

    x = 0;
    previous = millis();

    // this loop waits for the answer
    do
    {
        // if there are data in the UART input buffer, reads it and checks for the asnwer
        if (Serial1.available() != 0)
        {
            String inData = Serial1.readString();
            Serial.println(inData);

            inData.toCharArray(response, 300);

            //            response[x] = Serial1.read();
            //            x++;

            // Serial.println(response);
            // check if the desired answer 1  is in the response of the module
            if (strstr(response, expected_answer1) != NULL)
            {
                answer = 1;
            }
            // check if the desired answer 2 is in the response of the module
            else if (strstr(response, expected_answer2) != NULL)
            {
                answer = 2;
            }
        }
    }
    // Waits for the asnwer with time out
    while ((answer == 0) && ((millis() - previous) < timeout));

    return answer;
}

void MQTTconfigure()
{
    StaticJsonDocument<100> root;
    String lastwill_status;
    lastwill_status += "{";
    lastwill_status = lastwill_status + "'station_id':'";
    lastwill_status = lastwill_status + stationID;
    lastwill_status = lastwill_status + "',";
    lastwill_status = lastwill_status + "'status':";
    lastwill_status = lastwill_status + "'disconnected'"; 
    lastwill_status = lastwill_status + "}";
    
    resp = sendATcommand("AT+QMTCFG=\"WILL\",0,1,1,1,\"charge/lastwill\",\"" + lastwill_status + "\"", "OK", "ERROR", 1000);
    delay(1000);
    resp = sendATcommand("AT+QMTCFG=\"TIMEOUT\",0,15,3,1", "OK", "ERROR", 1000);
    delay(1000);
    resp = sendATcommand("AT+QMTCFG=\"KEEPALIVE\",0,5", "OK", "ERROR", 1000);
    if (resp == 1)
    {
        delay(1000);
        resp = sendATcommand("AT+QMTOPEN=0,\"broker.hivemq.com\",1883", "OK", "ERROR", 1000);
        if (resp == 1)
        {
            delay(1000);
            resp = sendATcommand("AT+QMTCONN=0,\"charging123\"", "OK", "ERROR", 1000);
            if (resp == 1)
            {
                delay(1000);
                resp = sendATcommand("AT+QMTCONN?", "OK", "ERROR", 1000);
                if (resp == 1)
                {
                    delay(1000);
                    lastwill_status = "";
                    root.clear();
                    root["station_id"] = stationID;
                    root["status"] = "connected";
                    serializeJson(root, lastwill_status);
                    // Serial.print("lastwill data is: " + lastwill_status);
                    resp = sendATcommand("AT+QMTPUB=0,1,1,1,\"charge/lastwill\"", "OK", "ERROR", 1000);
                    // sendATcommand("connected","OK","ERROR",100);
                    //sendATcommand("26","OK","ERROR",100);
                    Serial1.println(lastwill_status);
                    Serial1.write((26));
                    //         if(resp==1)
                    //       {
                    delay(1000);
                    resp = sendATcommand("AT+QMTSUB=0,1,\"chargingstation/charge\",1", "OK", "ERROR", 1000);
                    resp = sendATcommand("AT+QMTSUB=0,1,\"chargingstation/charge/ack\",1", "OK", "ERROR", 1000);
                    resp = sendATcommand("ATE0", "OK", "ERROR", 1000);
                    if (resp == 1)
                    {
                        Serial.println("topic subscribed ");
                    }
                    // }
                }
            }
        }
    }
}



boolean connectgprs()
{
    resp = sendATcommand("AT+QIDEACT=1", "OK", "ERROR", 1000);
    if (resp == 1)
    {
        resp = sendATcommand("AT+QICSGP=1,1,\"jionet\",\"\",\"\",0", "OK", "ERROR", 1000);
        if (resp == 1)
        {
            resp = sendATcommand("AT+QICSGP=?", "OK", "ERROR", 1000);
            if (resp == 1)
            {
                resp = sendATcommand("AT+QIACT=1", "OK", "ERROR", 1000);
                Serial.println(F("internet started"));
                return true;
            }

            else
            {
                Serial.println(F("INTERNET NOT STARTRED"));
            }
        }
    }

    // Serial.println("not deactivated");
    return false;
}

boolean GsmConnectModule()
{
    Serial.println(F("in connect"));
    //  delay(1000);
    //  Serial1.println("AT+QIDEACT");
    resp = sendATcommand("AT", "OK", "ERROR", 500);
    if (resp == 1)
    {
        resp = sendATcommand("AT+CMEE=2", "OK", "ERROR", 500);
        if (resp == 1)
        {
            resp = sendATcommand("AT+CFUN?", "OK", "ERROR", 500);
            if (resp == 1)
            {
                resp = sendATcommand("AT+CPIN?", "OK", "ERROR", 500);
                if (resp == 1)
                {
                    Serial.println("network registered");
                    return true;
                }
            }
        }
    }
    return false;
}

void GSM_INIT()
{
    if (GsmConnectModule())
    {
        if (connectgprs())
        {
            MQTTconfigure();
            return;
        }
    }
    else
    {
        //GSM_INIT();
    }
    Serial.println(F("Restart Mega"));
}