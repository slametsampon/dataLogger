/*
  logsheet.h - Library for DHT wrapper
  By : Sam Sept 04, 2021
*/

#ifndef logsheet_h
#define logsheet_h

#include <Arduino.h>
#include <ArduinoJson.h>
#include <DHT.h>
#include <Wire.h>  // Include Wire if you're using I2C
#include <SPI.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <LittleFS.h>

#include "model.h"

const boolean SIMULATION = true;
const int ALFA_EMA = 80;
const int DAY_366 = 366;
const int HOUR_24 = 24;
const int MINUTE_60 = 60;
const int SECOND_6 = 6;//average at last one
const int SECOND_60 = 60;//average at last one

const String PATH_LS = "/logsheet/";
const String SENSOR_FILE_CFG = "sensors.cfg";
const String HEADER = "TIME;TEMPERATURE;HUMIDITY\n";
const int DAILY_SIZE = 425 ;//26 + (16*24) = 410

enum DayOfWeek{
  Sunday, Monday, Tuesday, Wednesday, Thursday, Friday, Saturday
};
    class Logsheet{
        public:
            Logsheet(String);
            void AttachParameter(AccessParam*, AccessParam*);
            void AttachSensor(DHT*);
            void AttachDisplay(Adafruit_SSD1306*);
            String getHourlyAvg(int);
            String getCfgParameter();
            void setTime(struct tm);
            void execute(unsigned long);//sampling periode ms
            String getValues();//return string in format Json for Temp, Humidity
            void info();

        private:
            void _setupDefaultParameter();
            void _setupFileCfgParameter(String);
            void _getSensorValue();
            void _print(logsheetData);
            String _initRandomJson();
            void _oledDisplay(float, float);

            String _getTimeStr(int);
            String _getDayOfWeek(int);
            void _recordEvent();
            void _recordLogsheet();
            void _minuteLogsheet();
            void _hourlyLogsheet();
            void _dailyLogsheet();
            String _getCsv(logsheetData data);
            void _shiftArray(int size, logsheetData last);
            logsheetData _calculateAverage(logsheetData data[], int size);

            void _appendFile(const char * path, const char * message);
            void _writeFile(const char * path, const char * message);
            String _readFile(const char * path);
            String _readFileJson(int);//read file day of week, return json
            
            String _id;
            float _prevT, _prevH;//temperature and humidity
	          unsigned long _prevMilli, _samplingTime;
            int _samplingSec, _nbrSamplingSec;
            boolean _minuteEvent, _hourEvent, _dayEvent;
            boolean _saveHourlyEvent, _saveDailyEvent;
            boolean _synchronized = false;
            struct tm _tm;

            DHT *_dht;
            Adafruit_SSD1306 *_display;
            AccessParam *_paramTemperature, *_paramHumidity;
            logsheetData _logsheetSecond[SECOND_60];
            logsheetData _logsheetMinute[MINUTE_60];
    };

#endif
