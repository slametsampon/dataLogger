/*
  dhtWrapper.h - Library for DHT wrapper
  By : Sam Sept 04, 2021
*/

#ifndef dhtWrapper_h
#define dhtWrapper_h

#include <Arduino.h>
#include  <ArduinoJson.h>
#include <DHT.h>

#include "model.h"

    class DhtWrapper{
        public:
            DhtWrapper(DHT*);
            void AttachParameter(AccessParam*, AccessParam*);
            String getValues();//return string in format Json for Temp, Humidity
            void info();
        private:
            DHT *_dht;
            AccessParam *_paramTemperature, *_paramHumidity;
    };

#endif
