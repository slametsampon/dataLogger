/*
It is warpper for DHT sensor
by : Sam 04/09/2021

*/

#include "logsheet.h"

Logsheet::Logsheet(String id): _id(id){}

void Logsheet::AttachSensor(DHT *dht){
    _dht = dht;
    _setupDefaultParameter();
}

void Logsheet::AttachDisplay(Adafruit_SSD1306 *display){
    _display = display;
}

void Logsheet::AttachParameter(AccessParam *paramTemperature, AccessParam *paramHumidity){
    _paramTemperature = paramTemperature;
    _paramHumidity = paramHumidity;
}

void Logsheet::info(){
    Serial.println("Logsheet::info()");
    _paramTemperature->info();
    _paramHumidity->info(); 
}

String Logsheet::getValues(){
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

String Logsheet::getHour24(){
    return (this->_initRandomJson());
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
  _display->print(" °C ");
  _display->display();
}

void Logsheet::_setupDefaultParameter(){
  param dtParam;
  Serial.println("Logsheet::_setupDefaultParameter()");

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

String Logsheet::_initRandomJson(){
  String output;
  DynamicJsonDocument doc(1536);
  float h, t;
  doc.clear();
  JsonArray time = doc.createNestedArray("time");
  JsonArray temperature = doc.createNestedArray("temperature");
  JsonArray humidity = doc.createNestedArray("humidity");

  for (int i=0; i < 24; i++){
    h = random(400.0, 950)/10.0;
    t = random(200.0, 455)/10.0;

    time.add(i);
    temperature.add(t);
    humidity.add(h);
  } 

  serializeJson(doc, output);
  return output;
}

//LittleFS - Operations
void Logsheet::_readFile(const char * path) {
  Serial.printf("Reading file: %s\n", path);

  File file = LittleFS.open(path, "r");
  if (!file) {
    Serial.println("Failed to open file for reading");
    return;
  }

  Serial.print("Read from file: ");
  while (file.available()) {
    Serial.write(file.read());
  }
  file.close();
}

void Logsheet::_writeFile(const char * path, const char * message) {
  Serial.printf("Writing file: %s\n", path);

  File file = LittleFS.open(path, "w");
  if (!file) {
    Serial.println("Failed to open file for writing");
    return;
  }
  if (file.print(message)) {
    Serial.println("File written");
  } else {
    Serial.println("Write failed");
  }
  delay(2000); // Make sure the CREATE and LASTWRITE times are different
  file.close();
}

void Logsheet::_appendFile(const char * path, const char * message) {
  Serial.printf("Appending to file: %s\n", path);

  File file = LittleFS.open(path, "a");
  if (!file) {
    Serial.println("Failed to open file for appending");
    return;
  }
  if (file.print(message)) {
    Serial.println("Message appended");
  } else {
    Serial.println("Append failed");
  }
  file.close();
}

