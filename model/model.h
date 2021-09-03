/*
  Model.h - Library untuk Model
  Ini adalah bagian dari MVC (Model View Control) pattern design
  Dibuat Oleh : Sam Sept 02, 2021
  JSON

DynamicJsonDocument doc(1536);

JsonArray time = doc.createNestedArray("time");
time.add(0);
time.add(1);
...
...
...
time.add(22);
time.add(23);

JsonArray temperature = doc.createNestedArray("temperature");
temperature.add(-9999);
temperature.add(-9999);
...
...
...
temperature.add(-9999);
temperature.add(-9999);

JsonArray humidity = doc.createNestedArray("humidity");
humidity.add(-9999);
humidity.add(-9999);
...
...
...
humidity.add(-9999);
humidity.add(-9999);

serializeJson(doc, output);

? Stream& input;

DynamicJsonDocument doc(1536);

DeserializationError error = deserializeJson(doc, input);

if (error) {
  Serial.print(F("deserializeJson() failed: "));
  Serial.println(error.f_str());
  return;
}

JsonArray time = doc["time"];
int time_0 = time[0]; // 0
int time_1 = time[1]; // 1
...
...
...
int time_22 = time[22]; // 22
int time_23 = time[23]; // 23

JsonArray temperature = doc["temperature"];
int temperature_0 = temperature[0]; // -9999
int temperature_1 = temperature[1]; // -9999
...
...
...
int temperature_22 = temperature[22]; // -9999
int temperature_23 = temperature[23]; // -9999

JsonArray humidity = doc["humidity"];
int humidity_0 = humidity[0]; // -9999
int humidity_1 = humidity[1]; // -9999
...
...
...
int humidity_22 = humidity[22]; // -9999
int humidity_23 = humidity[23]; // -9999
*/
#ifndef model_h
#define model_h

#include "Arduino.h"
#include  <ArduinoJson.h>

  const int HOUR_24 = 24;
  const int MINUTE_60 = 60;
  const int SECOND_60 = 60;
  const int AVG_INDEX = 24;
  const int MIN_TEMP_INDEX = 25;
  const int MAX_TEMP_INDEX = 26;
  const int MIN_HUMD_INDEX = 27;
  const int MAX_HUMD_INDEX = 28;

  const int HOURLY_LOGSHEET = 29;
  const int PARAMETER_VALUE = 0;
  const int PARAMETER_LOW_RANGE = 1;
  const int PARAMETER_HIGH_RANGE = 2;
  const int PARAMETER_LOW_LIMIT = 3;
  const int PARAMETER_HIGH_LIMIT = 4;
  const int PARAMETER_ALFA_EMA = 5;
  const int PARAMETER_ALARM = 6;

    const int NO_ALARM = 0;
    const int HIGH_ALARM = 1;
    const int LOW_ALARM = 2;

  typedef struct dataLogsheet{
    int time;
    float temperature;
    float humidity;
  }dataLogsheet;

  class AccessDataLogsheet{
    public:
      AccessDataLogsheet(String );
      void add(dataLogsheet,int);
      dataLogsheet get(int);
      float getTemperature(int);
      float getHumidity(int);
      void saveHourlyLogsheet(JsonObject);
      void saveMinuteLogsheet(JsonObject);
      JsonObject readHourlyLogsheet();
      JsonObject readMinuteLogsheet();

      String toString();
      String getId();
      void info();

    private:
      dataLogsheet  _hourlyLogsheet[HOURLY_LOGSHEET];
      dataLogsheet  _minuteLogsheet[MINUTE_60];
      String    _id;
  };//end of class

    typedef struct param{
        String unit;//unit
        float value;
        float highRange;
        float lowRange;
        float highLimit;//for alarm high
        float lowLimit;//for alarm low
        float alfaEma;//alfa for EMA Filter (0 - 100) in percentage
        int alarm;
    }param;

  class AccessParam{
  
  public:
      AccessParam(String);
      void init(String, param);
      JsonObject getJson();
      JsonObject getOperation();
      param getParam();
      void setParamJson(JsonObject);
      void setParam(param);
      void setParam(int, float);
      void setAlarm(int);
      String toString();
      String getId();
      void info();

  private:
      String    _id;
      param _dataParam;

  };//end of class
#endif
