## class Logsheet
#### Public - functions:
1. Logsheet(String);
2. void AttachParameter(AccessParam*, AccessParam*);
3. void AttachSensor(DHT*);
4. void AttachDisplay(Adafruit_SSD1306*);
5. String getHourlyAvg(int);
6. String getCfgParameter();
7. void setTime(struct tm);
8. void execute(unsigned long);//sampling periode ms
9. String getValues();//return string in format Json for Temp, Humidity
10. void info();
#### Private - function:
1. void _setupDefaultParameter();
2. void _setupFileCfgParameter(String);
3. void _getSensorValue();
4. void _print(logsheetData);
5. String _initRandomJson();
6. void _oledDisplay(float, float);

7. String _getTimeStr(int);
8. String _getDayOfWeek(int);
9. void _recordEvent();
10. void _recordLogsheet();
11. void _minuteLogsheet();
12. void _hourlyLogsheet();
13. void _dailyLogsheet();
14. String _getCsv(logsheetData data);
15. void _shiftArray(int size, logsheetData last);
16. logsheetData _calculateAverage(logsheetData data[], int size);

17. void _appendFile(const char * path, const char * message);
18. void _writeFile(const char * path, const char * message);
19. String _readFile(const char * path);
20. String _readFileJson(int);//read file day of week, return json

#### Private - variables:
1. String _id;
2. float _prevT, _prevH;//temperature and humidity
    unsigned long _prevMilli, _samplingTime;
3. int _samplingSec, _nbrSamplingSec;
4. boolean _minuteEvent, _hourEvent, _dayEvent;
5. boolean _saveHourlyEvent, _saveDailyEvent;
6. boolean _synchronized = false;
7. struct tm _tm;

8. DHT *_dht;
9. Adafruit_SSD1306 *_display;
10. AccessParam *_paramTemperature, *_paramHumidity;
11. logsheetData _logsheetSecond[SECOND_60];
12. logsheetData _logsheetMinute[MINUTE_60];

#### Logsheet data: - file model.h
typedef struct logsheetData{
#### { 
1. String time;//HH:00:00 or DD_MM_YY
2. float temperature;
3. float humidity;
####
} logsheetData;

### Class ini bagian dari : [dataLogger](https://github.com/slametsampon/dataLogger/blob/main/config/dataLogger.md)
