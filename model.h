/*
  Model.h - Library untuk Model
  Ini adalah bagian dari MVC (Model View Control) pattern design
  Dibuat Oleh : Sam Feb 15, 2021
  JSON

StaticJsonDocument<96> doc;

doc["id"] = "Smoke-1";
doc["unit"] = "%";
doc["value"] = 51.5;
doc["highRange"] = 100;
doc["lowRange"] = 0;
doc["highLimit"] = 80;
doc["lowLimit"] = 40;
doc["alfaEma"] = 75.5;
doc["increment"] = 1.1;

serializeJson(doc, output);

? Stream& input;

StaticJsonDocument<192> doc;

DeserializationError error = deserializeJson(doc, input);

if (error) {
  Serial.print(F("deserializeJson() failed: "));
  Serial.println(error.f_str());
  return;
}

const char* id = doc["id"]; // "Smoke-1"
const char* unit = doc["unit"]; // "%"
float value = doc["value"]; // 51.5
int highRange = doc["highRange"]; // 100
int lowRange = doc["lowRange"]; // 0
int highLimit = doc["highLimit"]; // 80
int lowLimit = doc["lowLimit"]; // 40
float alfaEma = doc["alfaEma"]; // 75.5
float increment = doc["increment"]; // 1.1
*/
#ifndef model_h
#define model_h

#include "Arduino.h"
#include  <ArduinoJson.h>

  const int PARAMETER_VALUE = 0;
  const int PARAMETER_LOW_RANGE = 1;
  const int PARAMETER_HIGH_RANGE = 2;
  const int PARAMETER_LOW_LIMIT = 3;
  const int PARAMETER_HIGH_LIMIT = 4;
  const int PARAMETER_ALFA_EMA = 5;
  const int PARAMETER_INCREMENT = 6;
  const int PARAMETER_ALARM = 7;
  const int PARAMETER_OPERATION_MODE = 8;
  const float DELTA_INCREMENT = 0.5;

  //commSer header code
  const int DATA_OPERATION = 0;
  const int DATA_PARAMETER = 1;
  const int REMOTE_PARAMETER = 2;
  const int REMOTE_OPERATION = 3;

  const int NO_ALARM = 0;
  const int HIGH_ALARM = 1;
  const int LOW_ALARM = 2;

  typedef struct logsheetData{
      int time;
      float temperature;
      float humidity;
  }logsheetData;

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
      String getJson();
      JsonObject getOperation();
      void setOperationJson(JsonObject);
      param getParam();
      float getParam(int);
      void setParamJson(JsonObject);
      void setParam(param);
      void setParam(int, float);
      boolean isChangeAble(int);
      void setAlarm(int);
      String toString();
      String getId();
      void info();

  private:
      String    _id;
      param _dataParam;

  };//end of class
#endif
