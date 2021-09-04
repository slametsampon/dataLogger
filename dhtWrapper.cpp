/*
It is warpper for DHT sensor
by : Sam 04/09/2021

*/

#include "dhtWrapper.h"

DhtWrapper::DhtWrapper(DHT *dht): _dht(dht){}

void DhtWrapper::AttachParameter(AccessParam *paramTemperature,AccessParam *paramHumidity){
    _paramTemperature = paramTemperature;
    _paramHumidity = paramHumidity;
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
}