## Sensor Configuration
#### Sensor parameter:
typedef struct param{
#### { 
1. String unit;//unit
2. float value;
3. float highRange;
4. float lowRange;
5. float highLimit;//for alarm high
6. float lowLimit;//for alarm low
7. float alfaEma;//alfa for EMA Filter (0 - 100) in percentage
8. int alarm;
####
}param;

### class AccessParam
#### public - functions:
1. AccessParam(String);
2. void init(String, param);
3. String getJson();
4. JsonObject getOperation();
5. void setOperationJson(JsonObject);
6. param getParam();
7. float getParam(int);
8. void setParamJson(JsonObject);
9. void setParam(param);
10. void setParam(int, float);
11. boolean isChangeAble(int);
12. void setAlarm(int);
13. String toString();
14. String getId();
15. void info();

#### private - variables:
1. String    _id;
2. param _dataParam;

### Class ini bagian dari : [dataLogger](https://github.com/slametsampon/dataLogger/blob/main/config/dataLogger.md)
