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
    //_setupDefaultParameter();
    _setupFileCfgParameter(SENSOR_FILE_CFG);
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
  //return (this->_initRandomJson());
  return (this->_readFileJson(_tm.tm_wday));
}

void Logsheet::setTime(struct tm tmVal){
  _tm = tmVal;
  _tm.tm_year += 1900;
  _tm.tm_mon +=1;
  Serial.println("Logsheet::setTime(struct tm tmVal)");
  Serial.printf("Now is : %d-%02d-%02d %02d:%02d:%02d\n", _tm.tm_year, _tm.tm_mon, _tm.tm_mday, _tm.tm_hour, _tm.tm_min, _tm.tm_sec);
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

void Logsheet::_setupFileCfgParameter(String fileName){

  /*
    {
    "Temperature": {
      "unit":"°C",
      "value":25,
      "highRange":50.0,
      "lowRange":0.0,
      "highLimit":40.0,
      "lowLimit":10.0,
      "alfaEma":80.0,
      "alarm":0
    },
    "Humidity": {
      "unit":"%",
      "value":80,
      "highRange":100.0,
      "lowRange":0.0,
      "highLimit":90.0,
      "lowLimit":40.0,
      "alfaEma":80.0,
      "alarm":0
    }
  }

  StaticJsonDocument<384> doc;

  JsonObject Temperature = doc.createNestedObject("Temperature");
  Temperature["unit"] = "°C";
  Temperature["value"] = 25;
  Temperature["highRange"] = 50;
  Temperature["lowRange"] = 0;
  Temperature["highLimit"] = 40;
  Temperature["lowLimit"] = 10;
  Temperature["alfaEma"] = 80;
  Temperature["alarm"] = 0;

  JsonObject Humidity = doc.createNestedObject("Humidity");
  Humidity["unit"] = "%";
  Humidity["value"] = 80;
  Humidity["highRange"] = 100;
  Humidity["lowRange"] = 0;
  Humidity["highLimit"] = 90;
  Humidity["lowLimit"] = 40;
  Humidity["alfaEma"] = 80;
  Humidity["alarm"] = 0;

  serializeJson(doc, output);

  ! Stream& input;

  StaticJsonDocument<512> doc;

  DeserializationError error = deserializeJson(doc, input);

  if (error) {
    Serial.print(F("deserializeJson() failed: "));
    Serial.println(error.f_str());
    return;
  }

  JsonObject Temperature = doc["Temperature"];
  const char* Temperature_unit = Temperature["unit"]; // "°C"
  int Temperature_value = Temperature["value"]; // 25
  int Temperature_highRange = Temperature["highRange"]; // 50
  int Temperature_lowRange = Temperature["lowRange"]; // 0
  int Temperature_highLimit = Temperature["highLimit"]; // 40
  int Temperature_lowLimit = Temperature["lowLimit"]; // 10
  int Temperature_alfaEma = Temperature["alfaEma"]; // 80
  int Temperature_alarm = Temperature["alarm"]; // 0

  JsonObject Humidity = doc["Humidity"];
  const char* Humidity_unit = Humidity["unit"]; // "%"
  int Humidity_value = Humidity["value"]; // 80
  int Humidity_highRange = Humidity["highRange"]; // 100
  int Humidity_lowRange = Humidity["lowRange"]; // 0
  int Humidity_highLimit = Humidity["highLimit"]; // 90
  int Humidity_lowLimit = Humidity["lowLimit"]; // 40
  int Humidity_alfaEma = Humidity["alfaEma"]; // 80
  int Humidity_alarm = Humidity["alarm"]; // 0

  */
  param dtParam;
  Serial.println("_setupFileCfgParameter(String fileName)");

  String fullFileName = PATH_LS + fileName;

  Serial.print("fullFileName : ");
  Serial.println(fullFileName);

  char fileNameChar[31];
  fullFileName.toCharArray(fileNameChar,31);

  File file = LittleFS.open(fileNameChar, "r");
  if (!file) {
    Serial.println("Failed to open file for reading");
  }
  else{
    /*
    Serial.println("Contents : ");
    while (file.available()) {
      Serial.write(file.read());
    }
    Serial.println("");
    */

    StaticJsonDocument<512> doc;

    DeserializationError error = deserializeJson(doc, file);

    if (error) {
      Serial.print(F("deserializeJson() failed: "));
      Serial.println(error.f_str());
      return;
    }

    JsonObject Temperature = doc["Temperature"];
    //parameter temperature
    dtParam.unit = "°C";
    dtParam.value = Temperature["value"];
    dtParam.highRange = Temperature["highRange"];
    dtParam.lowRange = Temperature["lowRange"];
    dtParam.highLimit = Temperature["highLimit"];
    dtParam.lowLimit = Temperature["lowLimit"];
    dtParam.alfaEma = Temperature["alfaEma"];
    dtParam.alarm = Temperature["alarm"];
    _paramTemperature->setParam(dtParam);

    JsonObject Humidity = doc["Humidity"];
    //parameter humidity
    dtParam.unit = "%";
    dtParam.value = Humidity["value"];
    dtParam.highRange = Humidity["highRange"];
    dtParam.lowRange = Humidity["lowRange"];
    dtParam.highLimit = Humidity["highLimit"];
    dtParam.lowLimit = Humidity["lowLimit"];
    dtParam.alfaEma = Humidity["alfaEma"];
    dtParam.alarm = Humidity["alarm"];
    _paramHumidity->setParam(dtParam);

    file.close();
  }
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
    _minuteLogsheet();

    //testing
    //_dailyLogsheet();
  } 

  //handle hourly hour
  if (_hourEvent && !_saveHourlyEvent){
    _hourlyLogsheet();

    //testing
    //_dailyLogsheet();
  } 

  //handle daily logsheet
  if (_dayEvent && !_saveDailyEvent) _dailyLogsheet();
}

void Logsheet::_minuteLogsheet(){
  _minuteEvent = false;
  Serial.print("Logsheet::_recordLogsheet() => _samplingMinute : ");
  Serial.println(_tm.tm_min);

  //calculate average minute
  logsheetData avgMinute = _calculateAverage(_logsheetSecond, SECOND_6);

  //shift record minute & put average to bottom
  _shiftArray(MINUTE_60, avgMinute);
}

void Logsheet::_hourlyLogsheet(){
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

  if(!SIMULATION){
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
    }
  }
}

void Logsheet::_dailyLogsheet(){
  _saveDailyEvent = true;//reset

  Serial.print("Logsheet::_recordLogsheet() => _samplingDaily : ");
  Serial.printf("Now is : %d-%02d-%02d %02d:%02d:%02d\n", _tm.tm_year, _tm.tm_mon, _tm.tm_mday, _tm.tm_hour, _tm.tm_min, _tm.tm_sec);
  Serial.println("");

  //read data /logsheet/dayofweek_ls.csv
  String dayOfWeek = _getDayOfWeek(_tm.tm_wday);
  String fileName = dayOfWeek + "_ls.csv";
  fileName = PATH_LS + fileName;

  Serial.print("fileName : ");
  Serial.println(fileName);

  char fileNameChar[31], headerChar[50];
  fileName.toCharArray(fileNameChar,31);
  HEADER.toCharArray(headerChar,50);
  String fileContents = _readFile(fileNameChar);

  //parse data
  char contentsChar[DAILY_SIZE];
  fileContents.toCharArray(contentsChar,DAILY_SIZE);

  //Serial.println("contentsChar : ");
  //Serial.println(contentsChar);

  CSV_Parser cp(contentsChar, /**TIME;TEMPERATURE;HUMIDITY*/ "sss",true, /*delimiter*/ ';');

  //Serial.println("Accessing values by column name:");
  char **strTime = (char**)cp["TIME"];
  char **strT = (char**)cp["TEMPERATURE"];
  char **strH = (char**)cp["HUMIDITY"];
  
  //Serial.print("cp.getRowsCount() : ");
  //Serial.println(cp.getRowsCount());

  logsheetData hourlyAvg[cp.getRowsCount()];
  for(int row = 0; row < cp.getRowsCount(); row++) {
    if (SIMULATION){
      Serial.print(row, DEC);
      Serial.print(" => ");
      Serial.print(strTime[row]);
      Serial.print(" ; ");
      Serial.print(strT[row]);
      Serial.print(" ; ");
      Serial.println(strH[row]);
    }
    String Ti= String(strTime[row]);
    hourlyAvg[row].time = Ti;

    String T = String(strT[row]);
    hourlyAvg[row].temperature = T.toFloat();

    String H = String(strH[row]);
    hourlyAvg[row].humidity = H.toFloat();
  }

  //calculate average
  logsheetData avgDay = _calculateAverage(hourlyAvg, HOUR_24);

  //save daily average to file : /logsheet/ls_YYYY.csv (max 31 char)
  String year = _getTimeStr(_tm.tm_year);
  fileName ="";
  fileName = "ls_" + year;
  fileName  +=  ".csv";
  fileName = PATH_LS + fileName;

  Serial.print("fileName : ");
  Serial.println(fileName);

  fileName.toCharArray(fileNameChar,31);
  HEADER.toCharArray(headerChar,50);

  //create new file and clear data inside
  String data = _getCsv(avgDay);
  char dataChar[50];
  data.toCharArray(dataChar,50);
  //check filename
  if (!LittleFS.exists(fileNameChar)){
    _writeFile(fileNameChar, headerChar);
    _appendFile(fileNameChar, dataChar);
  }
  else _appendFile(fileNameChar, dataChar);
}

String Logsheet::_getCsv(logsheetData data){

  String csv = data.time + ";";

  csv += String(data.temperature);
  csv += ";";

  csv += String(data.humidity);
  csv += "\n";

  return csv;
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
    if (size == HOUR_24){
      //build DD_MM_YYYY
      String date = _getTimeStr(_tm.tm_mday);
      date += "_";
      date += _getTimeStr(_tm.tm_mon);
      date += "_";
      date += _getTimeStr(_tm.tm_year);
      avg.time = date;
    } 

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
String Logsheet::_readFile(const char * path) {
  String message = "";
  Serial.printf("Reading file: %s\n", path);

  File file = LittleFS.open(path, "r");
  if (!file) {
    Serial.println("Failed to open file for reading");
    return message;
  }

  int c = 0;
  Serial.println("Contents : ");
  while (file.available()) {
    c = file.read();
    message += (char) c;
    //Serial.write(file.read());
  }
  Serial.println("");
  file.close();

  return message;
}

String Logsheet::_readFileJson(int day_Week){
  //read data /logsheet/dayofweek_ls.csv
  String dayOfWeek = _getDayOfWeek(day_Week);
  String fileName = dayOfWeek + "_ls.csv";
  fileName = PATH_LS + fileName;

  Serial.print("fileName : ");
  Serial.println(fileName);

  char fileNameChar[31], headerChar[50];
  fileName.toCharArray(fileNameChar,31);
  String fileContents = _readFile(fileNameChar);

  //parse data
  char contentsChar[DAILY_SIZE];
  fileContents.toCharArray(contentsChar,DAILY_SIZE);

  CSV_Parser cp(contentsChar, /**TIME;TEMPERATURE;HUMIDITY*/ "sss",true, /*delimiter*/ ';');

  //Serial.println("Accessing values by column name:");
  char **strTime = (char**)cp["TIME"];
  char **strT = (char**)cp["TEMPERATURE"];
  char **strH = (char**)cp["HUMIDITY"];
  
  //Serial.print("cp.getRowsCount() : ");
  //Serial.println(cp.getRowsCount());

  logsheetData hourlyAvg[cp.getRowsCount()];

  //Json assembling
  String output;
  DynamicJsonDocument doc(1536);
  float h, t;
  doc.clear();
  JsonArray time = doc.createNestedArray("time");
  JsonArray temperature = doc.createNestedArray("temperature");
  JsonArray humidity = doc.createNestedArray("humidity");

  for(int row = 0; row < cp.getRowsCount(); row++) {
    if (SIMULATION){
      Serial.print(row, DEC);
      Serial.print(" => ");
      Serial.print(strTime[row]);
      Serial.print(" ; ");
      Serial.print(strT[row]);
      Serial.print(" ; ");
      Serial.println(strH[row]);
    }
    String Ti= String(strTime[row]);
    String T = String(strT[row]);
    String H = String(strH[row]);

    time.add(Ti.toInt());
    temperature.add(T.toFloat());
    humidity.add(H.toFloat());
  }

  serializeJson(doc, output);
  return output;
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
    Serial.println("File written");
  } else {
    Serial.println("Write failed");
  }
  delay(2000); // Make sure the CREATE and LASTWRITE times are different
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
    Serial.println("Message appended");
  } else {
    Serial.println("Append failed");
  }
  file.close();
}

