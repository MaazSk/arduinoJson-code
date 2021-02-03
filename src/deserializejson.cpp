#include "deserializeJson.hpp"
#include "MemoryFree.h"
void deserialization(char *data)
{
    Serial.print("before deserialization free memory is: ");
    Serial.println(freeMemory());
    DynamicJsonDocument doc(500);
    DeserializationError error = deserializeJson(doc, data);
    if (error)
    {
        Serial.print(F("deserializeJson() failed: "));
        Serial.println(error.c_str());
        return;
    }
    const char *tag;
    const char *request_id;
    const char *station_id;
    const char *plug_point;
    const char *energy_units;
    const char *date;
    const char *time;
    tag = doc["tag"];
    request_id = doc["request_id"];
    station_id = doc["station_id"];
    plug_point = doc["plug_point"];
    energy_units = doc["energy_units"];
    date = doc["date"];
    time = doc["time"];

    Serial.print(F("tag is: "));
    Serial.println(tag);

    Serial.print(F("request_id is: "));
    Serial.println(request_id);

    Serial.print(F("station_id is: "));
    Serial.println(station_id);

    Serial.print(F("plug_point is: "));
    Serial.println(plug_point);

    Serial.print(F("energy_units is: "));
    Serial.println(energy_units);

    Serial.print(F("date is: "));
    Serial.println(date);

    Serial.print(F("time is: "));
    Serial.println(time);

    // size_t outputSize = 400;
    DynamicJsonDocument root(400);
    char test[300];
    root["tag"] = tag;
    root["request_id"] = request_id;
    root["station_id"] = station_id;
    root["plug_point"] = plug_point;
    root["energy_units"] = energy_units;
    root["date"] = date;
    root["time"] = time;
    root["success"] = "success";
    serializeJson(root, test);
    // serializeJson(root, Serial);
    Serial.println(test);
    MQTTpublish(test);
    Serial.print("after deserialization free memory is: ");
    Serial.println(freeMemory());
}

void MQTTpublish(char *publishData)
{

    char *publishTopic = "AT+QMTPUB=0,1,1,0,\"charger/in/charge\"";
    Serial1.println(publishTopic); //Use of old method because the function is not working the code uses millis which skips the code.
    delay(400);
    // Serial.println("After waiting");
    Serial1.println(publishData);
    Serial1.write((26));
    Serial1.flush();
    // Serial.print(F("Topic is: "));
    // Serial.println(topic); //EDITED on 27-01-2021
    // delay(100);
}