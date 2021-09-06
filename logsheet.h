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

#include "model.h"

const boolean SIMULATION = true;
const int ALFA_EMA = 80;//AlfaEma filter

    class Logsheet{
        public:
            Logsheet(String);
            void AttachSensor(DHT*);
            void AttachDisplay(Adafruit_SSD1306*);
            String getValues();//return string in format Json for Temp, Humidity
            void info();

        private:
            void _setupDefaultParameter();
            void _oledDisplay(float, float);

            String _id;
            DHT *_dht;
            Adafruit_SSD1306 *_display;
            AccessParam *_paramTemperature, *_paramHumidity;
    };

#endif
