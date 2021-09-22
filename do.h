/*
  digitalOutput.h - Library for Digital Output (DO)
  Digital Input with debouncing mechanism 
  By : Sam March 03, 2021
*/

#ifndef do_h
#define do_h

#include <Arduino.h>

    const boolean FORWARD_TYPE = true;
    const boolean REVERSE_TYPE = false;
    const boolean PULLUP = true;
    const boolean PULLUP_NONE = false;

  class Do{
    public: 
      //constructor
      Do(int pin);  

      void init(boolean);
      void init(String);
      void init(boolean, String);
      boolean isStatus();
      void info();

      //typical for DO
      void on();
      void off();
      void blink(unsigned long);
    private:
      int  _pin;
      boolean _actionType;
      String _id, _device;
      unsigned long  _prevMilli;//in milli second
  };
#endif
