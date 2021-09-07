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
const int HOUR_24 = 24;
const int MINUTE_60 = 60;
const int SECOND_6 = 6;

    class Logsheet{
        public:
            Logsheet(String);
            void AttachParameter(AccessParam*, AccessParam*);
            void AttachSensor(DHT*);
            void AttachDisplay(Adafruit_SSD1306*);
            String getHour24();
            void execute(unsigned long);//sampling periode ms
            String getValues();//return string in format Json for Temp, Humidity
            void info();

        private:
            void _setupDefaultParameter();
            void _getSensorValue();
            String _initRandomJson();
            void _oledDisplay(float, float);
            void _appendFile(const char * path, const char * message);
            void _writeFile(const char * path, const char * message);
            void _readFile(const char * path);
            
            String _id;
            float _prevT, _prevH;//temperature and humidity
	        unsigned long _prevMilli;

            DHT *_dht;
            Adafruit_SSD1306 *_display;
            AccessParam *_paramTemperature, *_paramHumidity;
            logsheetData _logsheetSecond[SECOND_6];
            logsheetData _logsheetMinute[MINUTE_60];
            logsheetData _logsheetHour[HOUR_24];
    };

#endif
