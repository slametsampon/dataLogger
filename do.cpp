/*
 * lihat header file Do.h lebih detail
 * Digital Input with debouncing mechanism 
 * extention file harus .cpp bukan .CPP
 */

/* Dependencies */
#include "do.h"

Do::Do(int pin):_pin(pin), _id("Do"), _actionType(FORWARD_TYPE){
  _device = _id;
  this->init(_actionType);
  pinMode(_pin, OUTPUT);

}

void Do::init(boolean actType){
  _actionType = actType;

}

void Do::init(boolean actType, String id){
  _id = id;
  _actionType = actType;

}

boolean Do::isStatus(){

  if (_actionType == REVERSE_TYPE)return (!digitalRead(_pin));

  else return (digitalRead(_pin));
  
}

void Do::on(){
  if (_actionType == FORWARD_TYPE) digitalWrite(_pin, HIGH);
  else digitalWrite(_pin, LOW);
}

void Do::off(){
  if (_actionType == FORWARD_TYPE) digitalWrite(_pin, LOW);
  else digitalWrite(_pin, HIGH);
}

void Do::blink(unsigned long blinkTime){
    if ((millis() - _prevMilli) >= blinkTime){
        _prevMilli = millis();
        digitalWrite( _pin, digitalRead(_pin) ^ 1 );
    }
}

void Do::info(){
  Serial.print("_id : ");
  Serial.println(_id);

  Serial.print("_device : ");
  Serial.println(_device);

  Serial.print("Pin : ");
  Serial.println(_pin);

  Serial.print("Action : ");
  if (_actionType == FORWARD_TYPE) Serial.println("FORWARD");
  else Serial.println("REVERSE");

  Serial.print("Value : ");
  if (this->isStatus()) Serial.println("On");
  else Serial.println("Off");

  Serial.println("");
}
