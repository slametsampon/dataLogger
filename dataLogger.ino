/*
dataLogger - IoT data logger
by : Salman Alfarisi
*/

// Import required libraries
#include <Arduino.h>
#include <ESP8266WiFi.h>
#include <ESPAsyncTCP.h>
#include <ESPAsyncWebServer.h>
#include <ESP8266WiFiMulti.h>
#include <RTClib.h>

#include "dataLogger.h"
#include "SequenceTimer.h"
#include "logsheet.h"
#include "start_up.h"

// variables declaration - global
String loginSts = "FIRST_TIME";
unsigned long samplingTime = 0;

// objects declaration - global
//software RTC
RTC_Millis rtc;

StartUp startUp("startUp");

AccesUser accessEngineer("accessEngineer");
AccesUser accessOperator("accessOperator");
AccesUser activeUser("activeUser");

SequenceTimer mainSequence("mainSequence");
AccessParam accessParamTemperature("accessParamTemperature");
AccessParam accessParamHumidity("accessParamHumidity");

Adafruit_SSD1306 display(OLED_RESET);
DHT dhtSensor(DHTPIN, DHTTYPE);
Logsheet logsheet("logsheet");

ESP8266WiFiMulti wifiMulti; // Create an instance of the ESP8266WiFiMulti class, called 'wifiMulti'
// Create AsyncWebServer object on port 80
AsyncWebServer server(80);

//functions prototype
void setupRtc();
boolean authenticationUser(AsyncWebServerRequest *);
boolean validateParameter(AsyncWebServerRequest *);
void startWIFI_AP();
void startWiFiClient();
void startWiFiMulti();
void loadUsers();
void setupDefaultUser();
void handleLogin();
void handleConfig();
void listAllFilesInDir(String); //list files in all dir's
void urlController();
void loadStaticFile(); //css, js
String getBasicConfig();
String getDateTime();
void rtcSettingDT(String);

void setup()
{
  int step = 0;

  //init Oled display
  display.begin(SSD1306_SWITCHCAPVCC, 0x3C); // initialize with the I2C addr 0x3C (for the 64x48)
  startUp.AttachDisplay(&display);

  //display logo GMF
  startUp.logoDisplay();
  //welcome display
  startUp.welcomeDisplay();
  //init step
  step += 1;
  startUp.stepDisplay(step);

  //Setp up pin and serial
  pinMode(ledPin, OUTPUT);
  pinMode(BUILTIN_LED, OUTPUT);

  Serial.begin(115200);
  while (!Serial)
  {
    ;
  }

  if (DEBUG)
  {
    Serial.print(F("\n\nSerial started at 115200\n"));
  }
  /*increase step & display*/
  step += 1;
  startUp.stepDisplay(step);

  // Initialize LittleFS
  Serial.println("Begin LittleFS");
  if (!LittleFS.begin())
  {
    Serial.println("An Error has occurred while mounting LittleFS");
    return;
  }
  listAllFilesInDir("/");
  /*increase step & display*/
  step += 1;
  startUp.stepDisplay(step);

  //load Engineer, Operator and active user from littleFS
  loadUsers();
  accessEngineer.info();
  accessOperator.info();
  //setup default active user
  setupDefaultUser();
  activeUser.info();
  /*increase step & display*/
  step += 1;
  startUp.stepDisplay(step);

  // Initialize the sensor
  dhtSensor.begin();
  logsheet.AttachParameter(&accessParamTemperature, &accessParamHumidity);
  logsheet.AttachSensor(&dhtSensor);
  logsheet.AttachDisplay(&display);
  logsheet.info();
  /*increase step & display*/
  step += 1;
  startUp.stepDisplay(step);

  // Start WiFi
  if (AP_MODE)
    startWIFI_AP();
  //else startWiFiClient();
  else
    startWiFiMulti();
  /*increase step & display*/
  step += 1;
  startUp.stepDisplay(step);

  //url controller included load static css, js, images
  urlController();
  /*increase step & display*/
  step += 1;
  startUp.stepDisplay(step);

  // Start server
  server.begin();
  /*increase step & display*/
  step += 1;
  startUp.stepDisplay(step);

  //setup RTC
  setupRtc();
  rtcSettingDT(DEFAULT_RTC_DATE_TIME);
  logsheet.setTime(getTime());

  struct tm tmstruct = getTime();
  Serial.printf("\nNow is : %d-%02d-%02d %02d:%02d:%02d\n", tmstruct.tm_year, tmstruct.tm_mon, tmstruct.tm_mday, tmstruct.tm_hour, tmstruct.tm_min, tmstruct.tm_sec);
  Serial.println("");
  //setup samplingTime
  samplingTime = SAMPLING_TIME_DEFAULT; //default value
  Serial.printf("setup samplingTime : %3d\n", samplingTime);
  /*increase step & display*/
  step += 1;
  startUp.stepDisplay(step);
}

void loop()
{

  if (mainSequence.isAMinuteEvent())
    logsheet.setTime(getTime());

  //Logsheet action
  logsheet.execute(samplingTime);

  mainSequence.execute();
}

struct tm getTime()
{
  struct tm tmstruct;
  if (NTP_AVAILABLE)
  {
    Serial.println("Contacting Time Server");
    configTime(3600 * TIME_ZONE, DAY_SAVE_TIME * 3600, "time.nist.gov", "0.pool.ntp.org", "1.pool.ntp.org");
    delay(2000);
    tmstruct.tm_year = 0;
    ambilWaktuLokal(&tmstruct, 5000);

    //set offset to 1900, month +1
    tmstruct.tm_year += 1900;
    tmstruct.tm_mon += 1;
  }
  else
  {
    Serial.println("get time from RTC");
    DateTime now = rtc.now();
    tmstruct.tm_year = now.year();
    tmstruct.tm_mon = now.month();
    tmstruct.tm_mday = now.day();
    tmstruct.tm_wday = now.dayOfTheWeek();
    tmstruct.tm_hour = now.hour();
    tmstruct.tm_min = now.minute();
    tmstruct.tm_sec = now.second();
  }

  return tmstruct;
}

void startWiFiMulti()
{                                    // Try to connect to some given access points. Then wait for a connection
  wifiMulti.addAP(SSID1, PASSWORD1); // add Wi-Fi networks you want to connect to
  wifiMulti.addAP(SSID2, PASSWORD2);
  wifiMulti.addAP(SSID3, PASSWORD3);
  wifiMulti.addAP(SSID4, PASSWORD4);

  Serial.println("Connecting");
  while (wifiMulti.run() != WL_CONNECTED)
  { // Wait for the Wi-Fi to connect
    delay(250);
    Serial.print('.');
  }
  Serial.println("\r\n");
  Serial.print("Connected to ");
  Serial.println(WiFi.SSID()); // Tell us what network we're connected to
  Serial.print("IP address:\t");
  Serial.print(WiFi.localIP()); // Send the IP address of the ESP8266 to the computer
  Serial.println("\r\n");
}

void startWiFiClient()
{
  Serial.println("Connecting to " + (String)SSID1);
  WiFi.mode(WIFI_STA);
  WiFi.begin(SSID1, PASSWORD1);

  while (WiFi.status() != WL_CONNECTED)
  {
    delay(500);
    Serial.print(".");
  }
  Serial.println("");

  Serial.println("WiFi connected");
  Serial.println("IP address: " + WiFi.localIP().toString());
}

void startWIFI_AP()
{
  WiFi.mode(WIFI_AP);
  WiFi.softAP(SSID1, PASSWORD1);
  Serial.println("AP started");
  Serial.println("IP address: " + WiFi.softAPIP().toString());
}
void urlController()
{

  loadStaticFile();
  // Route for root / web page
  server.on("/", HTTP_GET, [](AsyncWebServerRequest *request)
            { request->send(LittleFS, "/index.html", "text/html"); });

  // route to report
  server.on("/report", HTTP_GET, [](AsyncWebServerRequest *request)
            {
              request->send(LittleFS, "/report.html", "text/html");
              loginSts = "FIRST_TIME";
            });

  // route to hourly average
  server.on("/hourlyAvgDay", HTTP_GET, [](AsyncWebServerRequest *request)
            {
              if (request->hasArg("days"))
              {
                String arg = request->arg("days");
                Serial.print("The days is: ");
                Serial.println(arg);

                int day = arg.toInt();

                String hourlyAvg = logsheet.getHourlyAvg(day);
                request->send(200, "application/json", hourlyAvg);
              }
              else
              {
                Serial.println("Post did not have a 'samplingTime' field.");
              }
            });

  // route to trending data
  server.on("/getTrendingData", HTTP_GET, [](AsyncWebServerRequest *request)
            {
              String trendData = logsheet.getTrendingData();
              request->send(200, "application/json", trendData);
            });

  // route to active user
  server.on("/getActiveUser", HTTP_GET, [](AsyncWebServerRequest *request)
            {
              String activeUsr = activeUser.getJson();
              request->send(200, "application/json", activeUsr);
              Serial.println(activeUsr);
            });

  // route to config sensor - temperature and humidity
  server.on("/getSensorCfg", HTTP_GET, [](AsyncWebServerRequest *request)
            {
              String sensorCfg = logsheet.getCfgParameter();
              request->send(200, "application/json", sensorCfg);
              Serial.println(sensorCfg);
            });

  // route to basic config
  server.on("/getBasicConfig", HTTP_GET, [](AsyncWebServerRequest *request)
            {
              String basicCfg = getBasicConfig();
              request->send(200, "application/json", basicCfg);
              Serial.println(basicCfg);
            });

  // route to getDateTime
  server.on("/getDateTime", HTTP_GET, [](AsyncWebServerRequest *request)
            {
              request->send_P(200, "text/plain", getDateTime().c_str());
              Serial.printf("getDateTime is:", getDateTime());
            });

  server.on("/getSamplingTime", HTTP_GET, [](AsyncWebServerRequest *request)
            {
              String strData = String(samplingTime);
              request->send_P(200, "text/plain", strData.c_str());
              Serial.printf("samplingTime is:%3d\n", samplingTime);
            });

  // route to update sensor - temperature and humidity
  server.on("/getSensor", HTTP_GET, [](AsyncWebServerRequest *request)
            {
              String strDhtVal = logsheet.getValues();
              request->send(200, "application/json", strDhtVal);
              Serial.println(strDhtVal);
            });

  // route to logout
  server.on("/logout", HTTP_GET, [](AsyncWebServerRequest *request)
            {
              //set active user to default
              setupDefaultUser();

              request->send(LittleFS, "/index.html", "text/html");
            });

  // route to login
  server.on("/login", HTTP_GET, [](AsyncWebServerRequest *request)
            { request->send(LittleFS, "/login.html", "text/html"); });

  // route to login - post
  server.on("/login", HTTP_POST, [](AsyncWebServerRequest *request)
            {
              //check user
              boolean valid = authenticationUser(request);
              if (valid)
                request->send(LittleFS, "/index.html", "text/html");
              else
                request->send(LittleFS, "/login.html", "text/html");
            });

  // route to loginStatus
  server.on("/loginStatus", HTTP_GET, [](AsyncWebServerRequest *request)
            {
              request->send_P(200, "text/plain", loginSts.c_str());
              Serial.printf("LoginSts is:%s\n", loginSts);
            });

  // route to login - post
  // route to config sensor - temperature and humidity
  server.on("/config", HTTP_GET, [](AsyncWebServerRequest *request)
            { request->send(LittleFS, "/config.html", "text/html"); });

  // route to config sensor - post - temperature and humidity
  server.on("/config", HTTP_POST, [](AsyncWebServerRequest *request)
            {
              //validate parameters
              boolean valid = validateParameter(request);

              if (valid)
                request->send(LittleFS, "/index.html", "text/html");
              else
                request->send(LittleFS, "/config.html", "text/html");
            });
}

boolean authenticationUser(AsyncWebServerRequest *request)
{
  String username, password;
  boolean status = false;
  loginSts = "VALID_LOGIN";

  if (request->hasArg("username"))
  {
    username = request->arg("username");
    Serial.print("The username is: ");
    Serial.println(username);
  }
  if (request->hasArg("password"))
  {
    password = request->arg("password");
    Serial.print("The password is: ");
    Serial.println(password);
  }
  else
  {
    Serial.println("Post did not have a 'username' field.");
  }

  //check username
  if ((username == accessEngineer.getUser(USER_NAME)) && (password == accessEngineer.getUser(USER_PASSWORD)))
  {
    status = true;
    activeUser.setUser(accessEngineer.getUser());
  }
  else if ((username == accessOperator.getUser(USER_NAME)) && (password == accessOperator.getUser(USER_PASSWORD)))
  {
    status = true;
    activeUser.setUser(accessOperator.getUser());
  }
  else
    loginSts = "INVALID_LOGIN";
  return status;
}

void rtcSettingDT(String dtValue)
{
  //set date and time
  int idx = dtValue.indexOf("T");

  String dateStr = dtValue.substring(0, idx);
  String timeStr = dtValue.substring(idx + 1);

  int yr = dateStr.substring(0, 4).toInt();
  int mt = dateStr.substring(5, 7).toInt();
  int dt = dateStr.substring(8).toInt();
  int hr = timeStr.substring(0, 2).toInt();
  int mn = timeStr.substring(3).toInt();

  // This line sets the RTC with an explicit date & time, for example to set
  // January 21, 2014 at 3am you would call:
  //rtc.adjust(DateTime(2014, 1, 21, 3, 0, 0));
  rtc.adjust(DateTime(yr, mt, dt, hr, mn, 0));
}

boolean validateParameter(AsyncWebServerRequest *request)
{
  boolean status = false;
  String argData;
  param dtParam;

  if (request->hasArg("samplingTime"))
  {
    argData = request->arg("samplingTime");
    Serial.print("The samplingTime is: ");
    Serial.println(argData);

    samplingTime = (unsigned long)argData.toInt();
    if (samplingTime < SAMPLING_TIME_MIN)
      samplingTime = (unsigned long)SAMPLING_TIME_MIN;
    else if (samplingTime > SAMPLING_TIME_MAX)
      samplingTime = (unsigned long)SAMPLING_TIME_MAX;
  }

  if (request->hasArg("enable_record"))
  {
    argData = request->arg("enable_record");
    Serial.print("The enable_record is: ");
    Serial.println(argData);

    if (argData == "Record")
      logsheet.setEnableRecord(true);
    else
      logsheet.setEnableRecord(false);
  }

  if (request->hasArg("date_time"))
  {
    argData = request->arg("date_time");
    Serial.print("The date_time is: ");
    Serial.println(argData);

    rtcSettingDT(argData);
  }

  if (request->hasArg("indicatorL"))
  {
    argData = request->arg("indicatorL");
    Serial.print("The indicatorL is: ");
    Serial.println(argData);

    float val = argData.toFloat();
    dtParam.lowRange = val;
  }

  if (request->hasArg("indicatorH"))
  {
    argData = request->arg("indicatorH");
    Serial.print("The indicatorH is: ");
    Serial.println(argData);

    float val = argData.toFloat();
    dtParam.highRange = val;
  }

  if (request->hasArg("alarmL"))
  {
    argData = request->arg("alarmL");
    Serial.print("The alarmL is: ");
    Serial.println(argData);

    float val = argData.toFloat();
    dtParam.lowLimit = val;
  }

  if (request->hasArg("alarmH"))
  {
    argData = request->arg("alarmH");
    Serial.print("The alarmH is: ");
    Serial.println(argData);

    float val = argData.toFloat();
    dtParam.highLimit = val;
  }

  if (request->hasArg("alfaEma"))
  {
    argData = request->arg("alfaEma");
    Serial.print("The alfaEma is: ");
    Serial.println(argData);

    float val = argData.toFloat();
    dtParam.alfaEma = val;
  }

  if (request->hasArg("config"))
  {
    argData = request->arg("config");
    Serial.print("The config is: ");
    Serial.println(argData);

    if (argData == "configT")
    {
      status = true;
      accessParamTemperature.setParam(dtParam);
    }

    else if (argData == "configH")
    {
      status = true;
      accessParamHumidity.setParam(dtParam);
    }
  }

  return status;
}

void loadStaticFile()
{
  // Route to load style.css file
  server.on("/style.css", HTTP_GET, [](AsyncWebServerRequest *request)
            { request->send(LittleFS, "/style.css", "text/css"); });

  // Route to load authentication.js file
  server.on("/authentication.js", HTTP_GET, [](AsyncWebServerRequest *request)
            { request->send(LittleFS, "/authentication.js", "text/js"); });

  // Route to load report.js file
  server.on("/report.js", HTTP_GET, [](AsyncWebServerRequest *request)
            { request->send(LittleFS, "/report.js", "text/js"); });

  // Route to load index.js file
  server.on("/index.js", HTTP_GET, [](AsyncWebServerRequest *request)
            { request->send(LittleFS, "/index.js", "text/js"); });

  // Route to load login.js file
  server.on("/login.js", HTTP_GET, [](AsyncWebServerRequest *request)
            { request->send(LittleFS, "/login.js", "text/js"); });

  // Route to load config.js file
  server.on("/config.js", HTTP_GET, [](AsyncWebServerRequest *request)
            { request->send(LittleFS, "/config.js", "text/js"); });

  // Route to load widgetsClass.js file
  server.on("/widgetsClass.js", HTTP_GET, [](AsyncWebServerRequest *request)
            { request->send(LittleFS, "/widgetsClass.js", "text/js"); });

  // Route to load modal-dialog.js file
  server.on("/modal-dialog.js", HTTP_GET, [](AsyncWebServerRequest *request)
            { request->send(LittleFS, "/modal-dialog.js", "text/js"); });

  // Route to load logoGMF file
  server.on("/logoGMF", HTTP_GET, [](AsyncWebServerRequest *request)
            { request->send(LittleFS, "/logoGMF.png", "image/png"); });
}

void listAllFilesInDir(String dir_path)
{
  Dir dir = LittleFS.openDir(dir_path);
  while (dir.next())
  {
    if (dir.isFile())
    {
      // print file names
      Serial.print("File: ");
      Serial.println(dir_path + dir.fileName());
    }
    if (dir.isDirectory())
    {
      // print directory names
      Serial.print("Dir: ");
      Serial.println(dir_path + dir.fileName() + "/");
      // recursive file listing inside new directory
      listAllFilesInDir(dir_path + dir.fileName() + "/");
    }
  }
}

void loadUsers()
{
  /*
  {
    "Engineer": {
      "username":"engineer",
      "password":"123456",
      "email":"engineer@example.com",
      "level":0
    },
    "Operator": {
      "username":"operator",
      "password":"123",
      "email":"operator@example.com",
      "level":9
    }
  }

  ! Stream& input;


  StaticJsonDocument<384> doc;

  DeserializationError error = deserializeJson(doc, input);

  if (error) {
    Serial.print(F("deserializeJson() failed: "));
    Serial.println(error.f_str());
    return;
  }

  JsonObject Engineer = doc["Engineer"];
  const char* Engineer_username = Engineer["username"]; // "engineer"
  const char* Engineer_password = Engineer["password"]; // "123456"
  const char* Engineer_email = Engineer["email"]; // "engineer@example.com"
  int Engineer_level = Engineer["level"]; // 0

  JsonObject Operator = doc["Operator"];
  const char* Operator_username = Operator["username"]; // "operator"
  const char* Operator_password = Operator["password"]; // "123"
  const char* Operator_email = Operator["email"]; // "operator@example.com"
  int Operator_level = Operator["level"]; // 9

  */
  userData userDt;
  Serial.println("_setupFileCfgParameter(String fileName)");

  String fullFileName = PATH_ROOT + FILE_USER;

  Serial.print("fullFileName : ");
  Serial.println(fullFileName);

  char fileNameChar[31];
  fullFileName.toCharArray(fileNameChar, 31);

  File file = LittleFS.open(fileNameChar, "r");
  if (!file)
  {
    Serial.println("Failed to open file for reading");
  }
  else
  {
    StaticJsonDocument<384> doc;

    DeserializationError error = deserializeJson(doc, file);

    if (error)
    {
      Serial.print(F("deserializeJson() failed: "));
      Serial.println(error.f_str());
      return;
    }

    JsonObject Engineer = doc["Engineer"];
    //Engineer
    userDt.username = (String)Engineer["username"];
    userDt.password = (String)Engineer["password"];
    userDt.email = (String)Engineer["email"];
    String level = (String)Engineer["level"];
    userDt.level = level.toInt();
    accessEngineer.setUser(userDt);

    JsonObject Operator = doc["Operator"];
    //Engineer
    userDt.username = (String)Operator["username"];
    userDt.password = (String)Operator["password"];
    userDt.email = (String)Operator["email"];
    level = (String)Operator["level"];
    userDt.level = level.toInt();
    accessOperator.setUser(userDt);

    file.close();
  }
}

void setupDefaultUser()
{
  userData userDt;
  userDt.username = "guest";
  userDt.password = "guest";
  userDt.email = "guest@example.com";
  userDt.level = 0;
  activeUser.setUser(userDt);
}

//NTP
bool ambilWaktuLokal(struct tm *info, uint32_t ms)
{
  uint32_t count = ms / 10;
  time_t now;

  time(&now);
  localtime_r(&now, info);

  if (info->tm_year > (2016 - 1900))
  {
    return true;
  }

  while (count--)
  {
    delay(10);
    time(&now);
    localtime_r(&now, info);
    if (info->tm_year > (2016 - 1900))
    {
      return true;
    }
  }
  return false;
}

//setup RTC
void setupRtc()
{
  // following line sets the RTC to the date & time this sketch was compiled
  rtc.begin(DateTime(F(__DATE__), F(__TIME__)));
  // This line sets the RTC with an explicit date & time, for example to set
  // January 21, 2014 at 3am you would call:
  // rtc.adjust(DateTime(2014, 1, 21, 3, 0, 0));
}

String getBasicConfig()
{
  /*
    {
      "dateTime": "2022-01-04T06:59",
      "samplingTime": 10000,
      "enable_record": "prohibited"
    }  
  */
  String output;

  StaticJsonDocument<96> doc;

  //set date time
  doc["dateTime"] = getDateTime();

  //set sampling time
  doc["samplingTime"] = samplingTime;

  //set enable record
  doc["enable_record"] = "Prohibited";
  if (logsheet.isEnableRecord())
    doc["enable_record"] = "Record";

  serializeJson(doc, output);

  return output;
}

String getDateTime()
{
  //set date time
  String date_time;
  struct tm currDT = getTime();

  date_time = String(currDT.tm_year) + "-";
  if (currDT.tm_mon < 10)
    date_time = date_time + "0";
  date_time = date_time + String(currDT.tm_mon);
  date_time = date_time + "-";

  if (currDT.tm_mday < 10)
    date_time = date_time + "0";
  date_time = date_time + String(currDT.tm_mday);
  date_time = date_time + "T";

  if (currDT.tm_hour < 10)
    date_time = date_time + "0";
  date_time = date_time + String(currDT.tm_hour);
  date_time = date_time + ":";

  if (currDT.tm_min < 10)
    date_time = date_time + "0";
  date_time = date_time + String(currDT.tm_min);

  return date_time;
}

/*  
  // Stores LED state
  String ledState;  
  // Route to set GPIO to HIGH
  server.on("/on", HTTP_GET, [](AsyncWebServerRequest *request){
    digitalWrite(ledPin, HIGH);    
    request->send(LittleFS, "/index.html", String(), false, processor);
  });
  
  // Route to set GPIO to LOW
  server.on("/off", HTTP_GET, [](AsyncWebServerRequest *request){
    digitalWrite(ledPin, LOW);    
    request->send(LittleFS, "/index.html", String(), false, processor);
  });

  server.on("/temperature", HTTP_GET, [](AsyncWebServerRequest *request){
    request->send_P(200, "text/plain", getTemperature().c_str());
  });
  
  server.on("/humidity", HTTP_GET, [](AsyncWebServerRequest *request){
    request->send_P(200, "text/plain", getHumidity().c_str());
  });

String getTemperature() {
  // float temperature = bme.readTemperature();
  float temperature = random(200, 455)/10.0;
  // Read temperature as Fahrenheit (isFahrenheit = true)
  //float t = dht.readTemperature(true);
  Serial.println(temperature);
  return String(temperature);
}
  
String getHumidity() {
  //float humidity = bme.readHumidity();
  float humidity = random(400.0, 950)/10.0;
  Serial.println(humidity);
  return String(humidity);
}

// Replaces placeholder with LED state value
String processor(const String& var){
  Serial.println(var);
  if(var == "STATE"){
    if(digitalRead(ledPin)){
      ledState = "ON";
    }
    else{
      ledState = "OFF";
    }
    Serial.print(ledState);
    return ledState;
  }
  else if (var == "TEMPERATURE"){
    return getTemperature();
  }
  else if (var == "HUMIDITY"){
    return getHumidity();
  }  
}
 
*/
