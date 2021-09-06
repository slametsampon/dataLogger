#include    "model.h"


//AccessParam - Class
AccessParam::AccessParam(String id):_id(id){}

void AccessParam::init(String id, param dtParam){
  _id = id;

  this->setParam(dtParam);
}

String AccessParam::getJson(){
  /*
  {
    "unit":"%",
    "value":51.5,
    "highRange":100.0,
    "lowRange":0.0,
    "highLimit":80.0,
    "lowLimit":40.0,
    "alfaEma":80.0,
    "alarm":2
  }
  StaticJsonDocument<192> doc;

  doc["unit"] = "%";
  doc["value"] = 51.5;
  doc["highRange"] = 100;
  doc["lowRange"] = 0;
  doc["highLimit"] = 80;
  doc["lowLimit"] = 40;
  doc["alfaEma"] = 80;
  doc["alarm"] = 2;

  serializeJson(doc, output);
  */

  // Get a reference to the root object
  StaticJsonDocument<192> doc;
  String output;
  
  doc["unit"] = _dataParam.unit;
  doc["value"] = _dataParam.value;
  doc["highRange"] = _dataParam.highRange;
  doc["lowRange"] = _dataParam.lowRange;
  doc["highLimit"] = _dataParam.highLimit;
  doc["lowLimit"] = _dataParam.lowLimit;
  doc["alfaEma"] = _dataParam.alfaEma;
  doc["alarm"] = _dataParam.alarm;

  serializeJson(doc, output);
  return output;
}

void  AccessParam::setParamJson(JsonObject paramJson){

  //_id = paramJson["id"];
  //_dataParam.unit = paramJson["unit"];

  _dataParam.value = paramJson["value"];
  _dataParam.highRange = paramJson["highRange"];
  _dataParam.lowRange = paramJson["lowRange"];
  _dataParam.highLimit = paramJson["highLimit"];
  _dataParam.lowLimit = paramJson["lowLimit"];
  _dataParam.alfaEma = paramJson["alfaEma"];
  _dataParam.alarm = paramJson["alarm"];

}

JsonObject AccessParam::getOperation(){
  /*
  {
    "unit":"%",
    "value":51.5,
    "operationMode":2,
    "alarm":2
  }

  StaticJsonDocument<96> doc;

  doc["unit"] = "%";
  doc["value"] = 51.5;
  doc["alarm"] = 2;

  serializeJson(doc, output);
  */

  StaticJsonDocument<96> doc;
  doc["unit"] = _dataParam.unit;
  doc["value"] = _dataParam.value;
  doc["alarm"] = _dataParam.alarm;

  return doc.as<JsonObject>();
}

param AccessParam::getParam(){
  return _dataParam;
}

float AccessParam::getParam(int idParam){
  switch (idParam)
  {
    case PARAMETER_VALUE:
      return _dataParam.value;
      break;

    case PARAMETER_LOW_RANGE:
      return _dataParam.lowRange;
      break;
    
    case PARAMETER_HIGH_RANGE:
      return _dataParam.highRange;
      break;
    
    case PARAMETER_LOW_LIMIT:
      return _dataParam.lowLimit;
      break;
    
    case PARAMETER_HIGH_LIMIT:
      return _dataParam.highLimit;
      break;
    
    case PARAMETER_ALARM:
      return (float) _dataParam.alarm;
      break;
    
    case PARAMETER_ALFA_EMA:
      return _dataParam.alfaEma;
      break;

    default:
      break;
  }
}

void AccessParam::setAlarm(int val){
  _dataParam.alarm =  val;
}

void AccessParam::setParam(param dataParam){

  _dataParam = dataParam;
}

boolean AccessParam::isChangeAble(int idParam){

  boolean isChangeAble = false;

  switch (idParam)
  {
    case PARAMETER_LOW_RANGE:
    case PARAMETER_HIGH_RANGE:
    case PARAMETER_LOW_LIMIT:
    case PARAMETER_HIGH_LIMIT:
    case PARAMETER_INCREMENT:
    case PARAMETER_ALFA_EMA:
      isChangeAble = true;
      break;
    
    default:
      break;
  }
  
  return isChangeAble;
}
void AccessParam::setParam(int idParam, float val){

  switch (idParam)
  {
    case PARAMETER_VALUE:
      _dataParam.value = val;
      break;

    case PARAMETER_LOW_RANGE:
      _dataParam.lowRange = val;
      break;
    
    case PARAMETER_HIGH_RANGE:
      _dataParam.highRange = val;
      break;
    
    case PARAMETER_LOW_LIMIT:
      _dataParam.lowLimit = val;
      break;
    
    case PARAMETER_HIGH_LIMIT:
      _dataParam.highLimit = val;
      break;
    
    case PARAMETER_ALARM:
      _dataParam.alarm = (int) val;
      break;
    
    case PARAMETER_ALFA_EMA:
      _dataParam.alfaEma = val;
      break;

    default:
      break;
  }
}

String AccessParam::getId(){
  return _id;
}

String AccessParam::toString(){
  String str = _id;
  String alm = "-N";

  str =  String(str + " ");
  str =  String(str + _dataParam.value);

  str =  String(str + _dataParam.unit);
  
  if (_dataParam.alarm == LOW_ALARM){
    alm = "-L";
  }
  if (_dataParam.alarm == HIGH_ALARM){
    alm = "-H";
  }
  str =  String(str + alm);
  return str;
}

void AccessParam::info(){
  // Get a reference to the root object
  StaticJsonDocument<192> paramJson;
  
  Serial.println("AccessParam::info()");

  String strJson = this->getJson();
  Serial.println(strJson);
  /*
  // Deserialize the JSON document in the response
  deserializeJson(paramJson, strJson);
  paramJson.as<JsonObject()>;

  Serial.println("Info parameter from Json");
  String str;
  for (JsonPair p : paramJson) {
    str = p.key().c_str();
    str = String (str + " : ");
    str = String (str + p.value().as<String>());
    Serial.println(str);
  }   
  */

}