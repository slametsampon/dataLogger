/*
It is warpper for DHT sensor
by : Sam 04/09/2021

*/

#include "logsheet.h"

Logsheet::Logsheet(String id): _id(id){
	_prevMilli = millis();
}

void Logsheet::AttachSensor(DHT *dht){
    _dht = dht;
    _setupDefaultParameter();
}

void Logsheet::AttachDisplay(Adafruit_SSD1306 *display){
    _display = display;
    _display->begin(SSD1306_SWITCHCAPVCC, 0x3C);  // initialize with the I2C addr 0x3C (for the 64x48)

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

    float t = _paramTemperature->getParam(PARAMETER_VALUE);
    int tInt = t * 100;
    t = tInt/100.0;//get 2 digits

    float h = _paramHumidity->getParam(PARAMETER_VALUE);
    int hInt = h * 100;
    h = hInt/100.0;//get 2 digits

    String statusT = "Normal";
    String statusH = "Normal";

    if (_paramTemperature->getParam(PARAMETER_ALARM) == HIGH_ALARM)statusT = "AlarmH";
    if (_paramHumidity->getParam(PARAMETER_ALARM) == HIGH_ALARM)statusH = "AlarmH";

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

    int tInt = t * 100;
    t = tInt/100.0;//get 2 digits

    int hInt = h * 100;
    h = hInt/100.0;//get 2 digits

  _display->clearDisplay();
  _display->setTextSize(1);
  
  _display->setTextColor(WHITE);
  _display->setCursor(0,0);
  _display->println("Humidity ");
  _display->setCursor(1, 12);
  _display->print(h);
  _display->print(" %\t");
  
  _display->setCursor(1, 21);
  _display->print("Temperatur");
  _display->setCursor(1, 30);
  _display->print(t);
  _display->print(" *C ");
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

void Logsheet::execute(unsigned long samplingTime){
	unsigned long currMilli = millis();

	if ((currMilli - _prevMilli) >= samplingTime){
		_prevMilli = currMilli;

    //get sensor data and update parameters
    this->_getSensorValue();

    //display to Oled
    this->_oledDisplay(_paramTemperature->getParam(PARAMETER_VALUE),
    _paramHumidity->getParam(PARAMETER_VALUE));

    //record event
    this->_recordEvent();

    //record logsheet
    this->_recordLogsheet();

	}
}

void Logsheet::_getSensorValue(){
  float tRaw,hRaw;//raw data

  float alfaEmaT = _paramTemperature->getParam(PARAMETER_ALFA_EMA);
  float alfaEmaH = _paramHumidity->getParam(PARAMETER_ALFA_EMA);

  float humidityAlarmH = _paramHumidity->getParam(PARAMETER_HIGH_LIMIT);
  float temperatureAlarmH = _paramTemperature->getParam(PARAMETER_HIGH_LIMIT);

  // get raw values from sensor
  if (!SIMULATION){
      tRaw = _dht->readTemperature(false);
      hRaw = _dht->readHumidity();
  }
  else{
      tRaw = random(200, 455)/10.0;
      hRaw = random(400.0, 950)/10.0;
  }

  //filtering raw value : AlfaEma filter
  float facCurrentT = alfaEmaT/100.0;
  float facPrevT = (100.0 - alfaEmaT)/100.0;
  float t = tRaw * facCurrentT + facPrevT * _prevT;

  float facCurrentH = alfaEmaH/100.0;
  float facPrevH = (100.0 - alfaEmaH)/100.0;
  float h = hRaw * facCurrentH + facPrevH * _prevH;

  if (_prevT != tRaw)_prevT = t;
  if (_prevH != hRaw)_prevH = h;

  //set value
  _paramTemperature->setParam(PARAMETER_VALUE, t);
  _paramHumidity->setParam(PARAMETER_VALUE, h);

  //set status
  if (t >= temperatureAlarmH){
    if (_paramTemperature->getParam(PARAMETER_ALARM) != HIGH_ALARM) _paramTemperature->setParam(PARAMETER_ALARM, HIGH_ALARM);
  }
  else _paramTemperature->setParam(PARAMETER_ALARM, NO_ALARM);

  if (h >= humidityAlarmH){
    if (_paramHumidity->getParam(PARAMETER_ALARM) != HIGH_ALARM) _paramHumidity->setParam(PARAMETER_ALARM, HIGH_ALARM);
  }
  else _paramHumidity->setParam(PARAMETER_ALARM, NO_ALARM);
}

void Logsheet::_recordEvent(){

  //handle sampling second
  if (_samplingSec >= (SECOND_6 - 1)){
    _samplingSec = 0;
    _recMinuteEvent = true;
  }
  else _samplingSec++;

  //handle sampling minute
  if (_recMinuteEvent){
    if (_samplingMinute >= (MINUTE_60 - 1)){
      _samplingMinute = 0;
      _recHourEvent = true;
    }
    else _samplingMinute++;
  }

  //handle sampling hour
  if (_recHourEvent){
    if (_samplingHour >= (HOUR_24 - 1)){
      _samplingHour = 0;
      _recDayEvent = true;
    }
    else _samplingHour++;
  }
}

void Logsheet::_recordLogsheet(){

  Serial.print("Logsheet::_recordLogsheet() => _samplingSec : ");
  Serial.println(_samplingSec);
  //shift record second
  logsheetData last;
  last.temperature = _paramTemperature->getParam(PARAMETER_VALUE);
  last.humidity = _paramHumidity->getParam(PARAMETER_VALUE);

  _shiftArray(_logsheetSecond, SECOND_6, last);

  //handle record Second
  if (_recMinuteEvent){
    _recMinuteEvent = false;//reset
    Serial.print("Logsheet::_recordLogsheet() => _samplingMinute : ");
    Serial.println(_samplingMinute);

    //calculate average minute
    logsheetData avgMinute = _calculateAverage(_logsheetSecond, SECOND_6);

    //shift record minute
    _shiftArray(_logsheetMinute, MINUTE_60, avgMinute);
  }

  //handle record hour
  if (_recHourEvent){
    _recHourEvent = false;//reset
    Serial.print("Logsheet::_recordLogsheet() => _samplingHour : ");
    Serial.println(_samplingHour);

    //calculate average hour
    logsheetData avgHour = _calculateAverage(_logsheetMinute, MINUTE_60);

    //save hourly average to file : /logsheet/ls_DD_MM_YY.csv (max 31 char) for 7 days

    //save daily average to file : /logsheet/ls_YYYY.csv (max 31 char)

  }

  //handle daily logsheet
  if (_recDayEvent){
    _recDayEvent = false;//reset
    Serial.println("Logsheet::_recordLogsheet() => _recDayEvent");

    //calculate average hour

    //save hourly average to file

  }
}

void Logsheet::_shiftArray(logsheetData data[], int size, logsheetData last){
  for (int i =0; i < (size - 1) ;i++){
    data[i].temperature = data[i+1].temperature;
    data[i].humidity = data[i+1].humidity;
    data[i].time = data[i+1].time;

    //Diagnostic
    /*
    Serial.print("i : ");
    Serial.print(i);
    Serial.print(", data[i].temperature : ");
    Serial.print(data[i].temperature);
    Serial.print(", data[i].humidity : ");
    Serial.println(data[i].humidity);
    */
  }
  data[size - 1].temperature = last.temperature;
  data[size - 1].humidity = last.humidity;
  data[size - 1].time = last.time;

/*
    Serial.print("[size - 1] : ");
    Serial.print(size - 1);
    Serial.print(", data[size - 1].temperature : ");
    Serial.print(data[size - 1].temperature);
    Serial.print(", data[size - 1].humidity : ");
    Serial.println(data[size - 1].humidity);
*/
}

logsheetData Logsheet::_calculateAverage(logsheetData data[], int size){
    float totalT, totalH;
    logsheetData avg;

    Serial.print("Logsheet::_calculateAverage() => size : ");
    Serial.println(size);

    for (int i =0; i < size  ;i++){
      totalT = totalT + _logsheetSecond[i].temperature;
      totalH = totalH + _logsheetSecond[i].humidity;

      //Diagnostic
      /*
      Serial.print("i : ");
      Serial.print(i);
      Serial.print(", _logsheetSecond[i].temperature : ");
      Serial.print(_logsheetSecond[i].temperature);
      Serial.print(", _logsheetSecond[i].humidity : ");
      Serial.println(_logsheetSecond[i].humidity);
      */
    }

    float avgT = totalT / size;
    float avgH = totalH / size;

    //Diagnostic
    /*
    Serial.print("avgT : ");
    Serial.print(avgT);
    Serial.print(", avgH : ");
    Serial.println(avgH);

    avg.temperature = avgT;
    avg.humidity = avgH;
    */

    return avg;
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

