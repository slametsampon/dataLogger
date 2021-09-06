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

#include "model.h"

const boolean SIMULATION = true;

    class Logsheet{
        public:
            Logsheet(DHT*);
            void AttachParameter(AccessParam*, AccessParam*);
            String getValues();//return string in format Json for Temp, Humidity
            void info();
        private:
            DHT *_dht;
            AccessParam *_paramTemperature, *_paramHumidity;
    };

#endif
