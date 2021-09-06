/*
It is warpper for DHT sensor
by : Sam 04/09/2021

*/

#include "dhtWrapper.h"

<<<<<<< HEAD:logsheet.cpp
Logsheet::Logsheet(String id): _id(id){
    this->_setupDefaultParameter();
}

void Logsheet::AttachSensor(DHT *dht){
    _dht = dht;
}

void Logsheet::AttachDisplay(Adafruit_SSD1306 *display){
    _display = display;
=======
DhtWrapper::DhtWrapper(DHT *dht): _dht(dht){}

void DhtWrapper::AttachParameter(AccessParam *paramTemperature,AccessParam *paramHumidity){
    _paramTemperature = paramTemperature;
    _paramHumidity = paramHumidity;
>>>>>>> parent of 43f1631 (work on logsheet):dhtWrapper.cpp
}

void DhtWrapper::info(){
    Serial.println("DhtWrapper::info()");
    _paramTemperature->info();
    _paramHumidity->info(); 
}

String DhtWrapper::getValues(){
    /*
    {
        "Temperature": {
        "value": 48.748010,
        "status": "AlarmH"
        },
        "Humidity": {
        "value": 70.86,
        "status": "Normal"
        }
    }
    */
    String output;
    StaticJsonDocument<128> doc;
    float t,h;
    // Read temperature as Fahrenheit (isFahrenheit = true, celsius = false)

    if (!SIMULATION){
        t = _dht->readTemperature(false);
        h = _dht->readHumidity();
    }
    else{
        t = random(200, 455)/10.0;
        h = random(400.0, 950)/10.0;
    }

    String statusT = "Normal";
    String statusH = "Normal";

    float humidityAlarmH = _paramHumidity->getParam(PARAMETER_HIGH_LIMIT);
    float temperatureAlarmH = _paramTemperature->getParam(PARAMETER_HIGH_LIMIT);

    if (t >= temperatureAlarmH)statusT = "AlarmH";
    if (h >= humidityAlarmH)statusH = "AlarmH";

    JsonObject Temperature = doc.createNestedObject("Temperature");
    Temperature["value"] = t;
    Temperature["status"] = statusT;

    JsonObject Humidity = doc.createNestedObject("Humidity");
    Humidity["value"] = h;
    Humidity["status"] = statusH;

    serializeJson(doc, output);

    return output;
<<<<<<< HEAD:logsheet.cpp
}

void Logsheet::_setupDefaultParameter(){
  param dtParam;
  Serial.println("LocPan::_setupParameter()");

  //parameter temperature
  dtParam.unit = "°C";
  dtParam.value = 35;
  dtParam.highRange = 50;
  dtParam.lowRange = -10;
  dtParam.highLimit = 40;
  dtParam.lowLimit = 10;
  dtParam.alfaEma = ALFA_EMA;
  dtParam.alarm = NO_ALARM;
  _paramTemperature->setParam(dtParam);

  //parameter humidity
  dtParam.unit = "%";
  dtParam.value = 70;
  dtParam.highRange = 100;
  dtParam.lowRange = 0;
  dtParam.highLimit = 90;
  dtParam.lowLimit = 40;
  dtParam.alfaEma = ALFA_EMA;
  dtParam.alarm = NO_ALARM;
  _paramHumidity->setParam(dtParam);
}

void Logsheet::_oledDisplay(float t, float h){
  _display->clearDisplay();

  _display->setTextSize(1);
  _display->setTextColor(WHITE);
  _display->setCursor(0,0);
  _display->println("Humidity: ");
  _display->setCursor(1, 12);
  _display->print(h);
  _display->print(" %\t");
  _display->setCursor(1, 21);
  _display->print("Temp :");
  _display->setCursor(1, 30);
  _display->print(t);
  _display->print(" *C ");

  _display->display();
}
=======
}
>>>>>>> parent of 43f1631 (work on logsheet):dhtWrapper.cpp
