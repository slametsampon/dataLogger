/*
It is warpper for DHT sensor
by : Sam 04/09/2021

*/

#include <CSV_Parser.h>
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

void Logsheet::setTime(struct tm tmVal){
  _tm = tmVal;
  Serial.printf("\nNow is : %d-%02d-%02d %02d:%02d:%02d\n", (_tm.tm_year) + 1900, (_tm.tm_mon) + 1, _tm.tm_mday, _tm.tm_hour, _tm.tm_min, _tm.tm_sec);
  Serial.println("");
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
    _minuteEvent = true;
  }
  else _samplingSec++;

  //handle sampling minute
  if (_tm.tm_min == 0){
    _hourEvent = true;
    Serial.print("_hourEvent : ");
    Serial.println(_hourEvent);
  }
  else{
    _hourEvent = false;
    _saveHourlyEvent = false;
  } 

  //handle sampling hour
  if (_tm.tm_hour == 0)_dayEvent = true;
  else{
    _dayEvent = false;
    _saveDailyEvent = false;
  } 
}

void Logsheet::_recordLogsheet(){

  Serial.print("Logsheet::_recordLogsheet() => _samplingSec : ");
  Serial.println(_samplingSec);
  //shift record second
  logsheetData last;
  last.temperature = _paramTemperature->getParam(PARAMETER_VALUE);
  last.humidity = _paramHumidity->getParam(PARAMETER_VALUE);
  last.time = _getTimeStr(_samplingSec);

  _shiftArray(SECOND_6, last);

  //handle record Second
  if (_minuteEvent){
    _minuteEvent = false;
    Serial.print("Logsheet::_recordLogsheet() => _samplingMinute : ");
    Serial.println(_tm.tm_min);

    //calculate average minute
    logsheetData avgMinute = _calculateAverage(_logsheetSecond, SECOND_6);

    //shift record minute & put average to bottom
    _shiftArray(MINUTE_60, avgMinute);
  }

  //handle hourly hour
  if (_hourEvent && !_saveHourlyEvent){
    _saveHourlyEvent = true;//reset

    Serial.print("Logsheet::_recordLogsheet() => _samplingHour : ");
    Serial.println(_tm.tm_hour);

    //calculate average hour & put average to bottom
    logsheetData avgHour = _calculateAverage(_logsheetMinute, MINUTE_60);

    //save hourly average to file : /logsheet/dayofweek_ls.csv (max 31 char) for 7 days
    String dayOfWeek = _getDayOfWeek(_tm.tm_wday);
    String fileName = dayOfWeek + "_ls.csv";
    fileName = PATH_LS + fileName;

    Serial.print("fileName : ");
    Serial.println(fileName);

    char fileNameChar[31], headerChar[50];
    fileName.toCharArray(fileNameChar,31);
    HEADER.toCharArray(headerChar,50);

    //create new file and clear data inside
    if (_tm.tm_hour == 0)_writeFile(fileNameChar, headerChar); 
    else{
      String data = _getCsv(avgHour);
      char dataChar[50];
      data.toCharArray(dataChar,50);
      //check filename
      if (!LittleFS.exists(fileNameChar)){
        _writeFile(fileNameChar, headerChar);
        _appendFile(fileNameChar, dataChar);
      }
      else _appendFile(fileNameChar, dataChar);

      //Serial.println(_readFile(fileNameChar));
    }
  }

  //handle daily logsheet
  if (_dayEvent && !_saveDailyEvent){
    _saveDailyEvent = true;//reset

    Serial.print("Logsheet::_recordLogsheet() => _samplingDaily : ");
    Serial.printf("\nNow is : %d-%02d-%02d %02d:%02d:%02d\n", (_tm.tm_year) + 1900, (_tm.tm_mon) + 1, _tm.tm_mday, _tm.tm_hour, _tm.tm_min, _tm.tm_sec);
    Serial.println("");

    //read data /logsheet/dayofweek_ls.csv

    //parse data

    //calculate average

    //save daily average to file : /logsheet/ls_YYYY.csv (max 31 char)
  }
}

String Logsheet::_getCsv(logsheetData data){

  String csv = data.time + ";";

  csv = csv + String(data.temperature);
  csv = csv + ";";

  csv = csv + String(data.humidity);

  return csv;
}

logsheetData Logsheet::_parseCsv(const char * csv_str){
  logsheetData data[HOUR_24];
  CSV_Parser cp(csv_str, /*time;temperature;humidity*/ "sff",true);
  Serial.println("Accessing values by column number:");

  //const String HEADER = "TIME;TEMPERATURE;HUMIDITY";
  char **time = (char**)cp["TIME"];
  float   *temperature = (float*)cp["TEMPERATURE"];
  float   *humidity = (float*)cp["HUMIDITY"];
  
  for(int row = 0; row < cp.getRowsCount(); row++) {
    Serial.print(row, DEC);
    Serial.print(". time = ");
    Serial.println(time[row]);
    Serial.print(row, DEC);
    Serial.print(". temperature = ");
    Serial.println(temperature[row]);
    Serial.print(". humidity = ");
    Serial.println(humidity[row]);
  }

  logsheetData test;
  return test;
}


void Logsheet::_shiftArray(int size, logsheetData last){

  switch (size)
  {
  case SECOND_6:
      for (int i =0; i < (size - 1) ;i++){
        _logsheetSecond[i].temperature = _logsheetSecond[i+1].temperature;
        _logsheetSecond[i].humidity = _logsheetSecond[i+1].humidity;
        _logsheetSecond[i].time = _logsheetSecond[i+1].time;

        //print to serial
        //_print(_logsheetSecond[i]);

      }
      _logsheetSecond[size - 1].temperature = last.temperature;
      _logsheetSecond[size - 1].humidity = last.humidity;
      _logsheetSecond[size - 1].time = last.time;

      //print to serial
      //_print(_logsheetSecond[size - 1]);
    break;

  case MINUTE_60:
      for (int i =0; i < (size - 1) ;i++){
        _logsheetMinute[i].temperature = _logsheetMinute[i+1].temperature;
        _logsheetMinute[i].humidity = _logsheetMinute[i+1].humidity;
        _logsheetMinute[i].time = _logsheetMinute[i+1].time;

        //print to serial
        //_print(_logsheetMinute[i]);

      }
      _logsheetMinute[size - 1].temperature = last.temperature;
      _logsheetMinute[size - 1].humidity = last.humidity;
      _logsheetMinute[size - 1].time = last.time;

      //print to serial
     //_print(_logsheetMinute[size - 1]);
    break;
  default:
    break;
  }
}

logsheetData Logsheet::_calculateAverage(logsheetData data[], int size){
    float totalT, totalH;
    logsheetData avg;
    int nbrData=0;

    Serial.print("Logsheet::_calculateAverage() => size : ");
    Serial.println(size);

    for (int i =0; i < size  ;i++){
      if ((data[i].temperature > 0)||(data[i].humidity > 0)){
        totalT = totalT + data[i].temperature;
        totalH = totalH + data[i].humidity;
        nbrData++;
        _print(data[i]);
      }
    }

    float avgT, avgH;
    if ((nbrData > 0) && (nbrData <= size)){
      avgT = totalT / nbrData;
      avgH = totalH / nbrData;
    }

    avg.temperature = avgT;
    avg.humidity = avgH;

    if (size == SECOND_6)avg.time = _getTimeStr(_tm.tm_min);
    if (size == MINUTE_60)avg.time = _getTimeStr(_tm.tm_hour);

    Serial.println("Average : ");
    _print(avg);

    return avg;
}

void Logsheet::_print(logsheetData data){
  String str = data.time;

  str =  String(str + "=> Temperature : ");
  str =  String(str + data.temperature);

  str =  String(str + ", Humidity : ");
  str =  String(str + data.humidity);

  Serial.println(str);
}

String Logsheet::_getTimeStr(int time){
  String str;

  if(time<10)str = "0" + String(time);
  else str = String(time);

  return str;
}

String Logsheet::_getDayOfWeek(int dayNumber){
  String dayOfWeek;
  switch (dayNumber)
  {
  case Monday:
      dayOfWeek = "Monday";
    break;
  
  case Tuesday:
      dayOfWeek = "Tuesday";
    break;
  
  case Wednesday:
      dayOfWeek = "Wednesday";
    break;
  
  case Thursday:
      dayOfWeek = "Thursday";
    break;
  
  case Friday:
      dayOfWeek = "Friday";
    break;
  
  case Saturday:
      dayOfWeek = "Saturday";
    break;
  
  case Sunday:
      dayOfWeek = "Sunday";
    break;
  
  default:
    break;
  }

  return dayOfWeek;
}
//LittleFS - Operations
char * Logsheet::_readFile(const char * path) {
  char * message;
  Serial.printf("Reading file: %s\n", path);

  File file = LittleFS.open(path, "r");
  if (!file) {
    Serial.println("Failed to open file for reading");
    return message;
  }

  Serial.print("Read from file: ");
  int i =0;
  while (file.available()) {
    int c = file.read();
    message[i] = c;
    i++;
  }
  return message;
}

void Logsheet::_writeFile(const char * path, const char * message) {
  Serial.printf("Writing file: %s\n", path);

  File file = LittleFS.open(path, "w");
  if (!file) {
    Serial.println("Failed to open file for writing");
    return;
  }
  if (file.print(message)) {
    Serial.print("message : ");
    Serial.print(message);
    Serial.println(", File written");
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
    Serial.print("message : ");
    Serial.print(message);
    Serial.println(", Message appended");
  } else {
    Serial.println("Append failed");
  }
  file.close();
}

