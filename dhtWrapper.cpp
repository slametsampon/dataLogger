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

String DhtWrapper::getDhtValues(){
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
    // Read temperature as Fahrenheit (isFahrenheit = true, celsius = false)
    float t = _dht->readTemperature(false);
    float h = _dht->readHumidity();

    String statusT = "Normal";
    String statusH = "Normal";

    float humidityAlarmH = _paramHumidity->getParam(PARAMETER_HIGH_RANGE);
    float temperatureAlarmH = _paramTemperature->getParam(PARAMETER_HIGH_RANGE);

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