/*
  dhtWrapper.h - Library for DHT wrapper
  By : Sam Sept 04, 2021
*/

#ifndef dhtWrapper_h
#define dhtWrapper_h

#include <Arduino.h>
#include  <ArduinoJson.h>
#include <DHT.h>
<<<<<<< HEAD:logsheet.h
#include <Wire.h>  // Include Wire if you're using I2C
#include <SPI.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
=======
>>>>>>> parent of 43f1631 (work on logsheet):dhtWrapper.h

#include "model.h"

const boolean SIMULATION = true;
const int ALFA_EMA = 80;//AlfaEma filter

    class DhtWrapper{
        public:
<<<<<<< HEAD:logsheet.h
            Logsheet(String);
            void AttachSensor(DHT*);
            void AttachDisplay(Adafruit_SSD1306*);
=======
            DhtWrapper(DHT*);
            void AttachParameter(AccessParam*, AccessParam*);
>>>>>>> parent of 43f1631 (work on logsheet):dhtWrapper.h
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
