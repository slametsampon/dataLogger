/*
  Rui Santos
  Complete project details at https://randomnerdtutorials.com/esp8266-web-server-spiffs-nodemcu/
  
  Permission is hereby granted, free of charge, to any person obtaining a copy
  of this software and associated documentation files.
  
  The above copyright notice and this permission notice shall be included in all
  copies or substantial portions of the Software.
*/

// Import required libraries
#include <ESP8266WiFi.h>
#include <ESPAsyncTCP.h>
#include <ESP8266mDNS.h>
#include <ESPAsyncWebServer.h>
#include <WebSocketsServer.h>
#include <ESP8266WiFiMulti.h> 
#include <ESP_Mail_Client.h>

#include "dataLogger.h"
#include "SequenceTimer.h"
#include "logsheet.h"

AccesUser accessEngineer("accessEngineer");
AccesUser accessOperator("accessOperator");
AccesUser activeUser("activeUser");

SequenceTimer   mainSequence("mainSequence");

AccessParam accessParamTemperature("accessParamTemperature");
AccessParam accessParamHumidity("accessParamHumidity");

Adafruit_SSD1306 display(OLED_RESET);
DHT dht(DHTPIN, DHTTYPE);
Logsheet logsheet("logsheet");

ESP8266WiFiMulti wifiMulti;     // Create an instance of the ESP8266WiFiMulti class, called 'wifiMulti'
// Create AsyncWebServer object on port 80
AsyncWebServer server(80);

WebSocketsServer webSocket = WebSocketsServer(81);
// it takes a short while for the websocket to initise fully. If you send the data too early it will be ignored.
// webSocketIsOpen is used to show the websocket is ready.
boolean webSocketIsOpen = false;

/* The SMTP Session object used for Email sending */
SMTPSession smtp;

/* Declare the message class */
SMTP_Message message;

//functions prototype
void startWiFiAP();
void startWiFiClient();
void startWiFiMulti();
void startMDNS();
void loadUsers();
void setupDefaultUser();

void urlController();
void handleLogin();
void handleConfig();
void loadStaticFile();//css, js
void listAllFilesInDir(String);//list files in all dir's
/* Callback function to get the Email sending status */
void smtpCallback(SMTP_Status status);

void setup(){
  // Serial port for debugging purposes
  Serial.begin(115200);
  while (!Serial) {;}
  if (DEBUG) { Serial.print(F("\n\nSerial started at 115200\n" ));   }

  // Initialize LittleFS
  Serial.println("Begin LittleFS");
  if(!LittleFS.begin()){
    Serial.println("An Error has occurred while mounting LittleFS");
    return;
  }
  listAllFilesInDir("/");

  //load Engineer and Operator from littleFS
  loadUsers();
  accessEngineer.info();
  accessOperator.info();

  //setup default active user
  setupDefaultUser();
  activeUser.info();

  pinMode(ledPin, OUTPUT);

  // Initialize the sensor
  logsheet.AttachParameter(&accessParamTemperature, &accessParamHumidity);
  logsheet.AttachSensor(&dht);
  logsheet.AttachDisplay(&display);
  logsheet.info();

  // Start WiFi
  if (WiFiAP) startWiFiAP();
  //else startWiFiClient();
  else startWiFiMulti();

  struct tm tmstruct = getTimeNtp();
  tmstruct.tm_year += 1900;
  tmstruct.tm_mon +=1;
  Serial.printf("\nNow is : %d-%02d-%02d %02d:%02d:%02d\n", tmstruct.tm_year, tmstruct.tm_mon, tmstruct.tm_mday, tmstruct.tm_hour, tmstruct.tm_min, tmstruct.tm_sec);
  Serial.println("");

  //logsheet setTime
  logsheet.setTime(getTimeNtp());

  //start mDNS
  startMDNS();

  //url controller
  urlController();

  // Start server
  server.begin();

}
 
void loop(){

  //Logsheet action
  logsheet.execute(SAMPLING_TIME);

  mainSequence.execute();
  
  if (mainSequence.isAMinuteEvent())logsheet.setTime(getTimeNtp());
  
}

struct tm getTimeNtp(){
  Serial.println("Contacting Time Server");
  configTime(3600 * timezone, daysavetime * 3600, "time.nist.gov", "0.pool.ntp.org", "1.pool.ntp.org");
  struct tm tmstruct;
  delay(2000);
  tmstruct.tm_year = 0;
  getLocalTime(&tmstruct, 5000);
  return tmstruct;
}

void startWiFiMulti() { // Try to connect to some given access points. Then wait for a connection
  wifiMulti.addAP(ssid1,password1);   // add Wi-Fi networks you want to connect to
  wifiMulti.addAP(ssid2,password2);
  wifiMulti.addAP(ssid3,password3);
  wifiMulti.addAP(ssid4,password4);

  Serial.println("Connecting");
  while (wifiMulti.run() != WL_CONNECTED) {  // Wait for the Wi-Fi to connect
    delay(250);
    Serial.print('.');
  }
  Serial.println("\r\n");
  Serial.print("Connected to ");
  Serial.println(WiFi.SSID());             // Tell us what network we're connected to
  Serial.print("IP address:\t");
  Serial.print(WiFi.localIP());            // Send the IP address of the ESP8266 to the computer
  Serial.println("\r\n");
}

void startWiFiClient(){
  Serial.println("Connecting to "+(String)ssid1);
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid1, password1);
  
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("");
  
  Serial.println("WiFi connected");
  Serial.println("IP address: " + WiFi.localIP().toString());
}

void startWiFiAP(){
  WiFi.mode(WIFI_AP);
  WiFi.softAP(ssid1, password1);
  Serial.println("AP started");
  Serial.println("IP address: " + WiFi.softAPIP().toString());
}

void startMDNS() { // Start the mDNS responder
  MDNS.begin(mdnsName);                        // start the multicast domain name server
  Serial.print("mDNS responder started: http://");
  Serial.print(mdnsName);
  Serial.println(".local");
}

void urlController(){

  loadStaticFile();
  // Route for root / web page
  server.on("/", HTTP_GET, [](AsyncWebServerRequest *request){
    request->send(LittleFS, "/index.html", "text/html");
  });

  server.on("/report", HTTP_GET, [](AsyncWebServerRequest *request){
    request->send(LittleFS, "/report.html", "text/html");
  });
  
  server.on("/hourlyAvgDay", HTTP_GET, [](AsyncWebServerRequest * request){
    if(request->hasArg("days")){
        String arg = request->arg("days");
        Serial.print("The days is: ");
        Serial.println(arg);

        int day = arg.toInt();

        String hourlyAvg = logsheet.getHourlyAvg(day);
        request->send(200, "application/json", hourlyAvg);
    } else {
        Serial.println("Post did not have a 'samplingTime' field.");
    }
  });

  // route to active user
  server.on("/getActiveUser", HTTP_GET, [](AsyncWebServerRequest *request){
    String activeUsr = activeUser.getJson();
    request->send(200, "application/json", activeUsr);
    Serial.println(activeUsr);
  });
  
  // route to config sensor - temperature and humidity
  server.on("/getSensorCfg", HTTP_GET, [](AsyncWebServerRequest *request){
    String sensorCfg = logsheet.getCfgParameter();
    request->send(200, "application/json", sensorCfg);
    Serial.println(sensorCfg);
  });
  
  // route to update sensor - temperature and humidity
  server.on("/getSensor", HTTP_GET, [](AsyncWebServerRequest *request){
    String strDhtVal = logsheet.getValues();
    request->send(200, "application/json", strDhtVal);
    Serial.println(strDhtVal);
  });
  
  // route to logout
  server.on("/logout", HTTP_GET, [](AsyncWebServerRequest * request){

    //set active user to default
    setupDefaultUser();

    request->send(LittleFS, "/index.html", "text/html");
  });

  // route to login
  server.on("/login", HTTP_GET, [](AsyncWebServerRequest * request){
    request->send(LittleFS, "/login.html", "text/html");
  });

  // route to login - post
  server.on("/login", HTTP_POST, [](AsyncWebServerRequest * request){
    //check user
    boolean valid = authenticationUser(request);
    if (valid) request->send(LittleFS, "/index.html", "text/html");
    else request->send(LittleFS, "/login.html", "text/html");

  });

  // route to config sensor - temperature and humidity
  server.on("/config", HTTP_GET, [](AsyncWebServerRequest *request){
    request->send(LittleFS, "/config.html", "text/html");
  });
  
  // route to config sensor - post - temperature and humidity
  server.on("/config", HTTP_POST, [](AsyncWebServerRequest * request){
    //validate parameters
    boolean valid = validateParameter(request);

    if (valid) request->send(LittleFS, "/index.html", "text/html");
    else request->send(LittleFS, "/config.html", "text/html");

  });
}

/* Callback function to get the Email sending status */
void smtpCallback(SMTP_Status status){
  /* Print the current status */
  Serial.println(status.info());

  /* Print the sending result */
  if (status.success()){
    Serial.println("----------------");
    ESP_MAIL_PRINTF("Message sent success: %d\n", status.completedCount());
    ESP_MAIL_PRINTF("Message sent failled: %d\n", status.failedCount());
    Serial.println("----------------\n");
    struct tm dt;

    for (size_t i = 0; i < smtp.sendingResult.size(); i++){
      /* Get the result item */
      SMTP_Result result = smtp.sendingResult.getItem(i);
      time_t ts = (time_t)result.timestamp;
      localtime_r(&ts, &dt);

      ESP_MAIL_PRINTF("Message No: %d\n", i + 1);
      ESP_MAIL_PRINTF("Status: %s\n", result.completed ? "success" : "failed");
      ESP_MAIL_PRINTF("Date/Time: %d/%d/%d %d:%d:%d\n", dt.tm_year + 1900, dt.tm_mon + 1, dt.tm_mday, dt.tm_hour, dt.tm_min, dt.tm_sec);
      ESP_MAIL_PRINTF("Recipient: %s\n", result.recipients);
      ESP_MAIL_PRINTF("Subject: %s\n", result.subject);
    }
    Serial.println("----------------\n");
  }
}

boolean authenticationUser(AsyncWebServerRequest * request){
  String username, password;
  boolean status = false;

  if(request->hasArg("username")){
      username = request->arg("username");
      Serial.print("The username is: ");
      Serial.println(username);
  }
  if(request->hasArg("password")){
      password = request->arg("password");
      Serial.print("The password is: ");
      Serial.println(password);
  }
  else {
      Serial.println("Post did not have a 'username' field.");
  }

  //check username
  if((username == accessEngineer.getUser(USER_NAME)) && (password == accessEngineer.getUser(USER_PASSWORD))){
    status = true;
    activeUser.setUser(accessEngineer.getUser());
  }
  else if((username == accessOperator.getUser(USER_NAME)) && (password == accessOperator.getUser(USER_PASSWORD))){
    status = true;
    activeUser.setUser(accessOperator.getUser());
  }
  return status;
}

boolean validateParameter(AsyncWebServerRequest * request){
  boolean status = false;
  String argData;
  param dtParam;

  if(request->hasArg("samplingTime")){
      String argData = request->arg("samplingTime");
      Serial.print("The samplingTime is: ");
      Serial.println(argData);

      float val = argData.toFloat();
  } 
  else if(request->hasArg("indicatorL")){
      String argData = request->arg("indicatorL");
      Serial.print("The indicatorL is: ");
      Serial.println(argData);

      float val = argData.toFloat();
      dtParam.lowRange = val;
  } 

  else if(request->hasArg("indicatorL")){
      String argData = request->arg("indicatorL");
      Serial.print("The indicatorL is: ");
      Serial.println(argData);

      float val = argData.toFloat();
      dtParam.lowRange = val;
  } 

  else if(request->hasArg("indicatorH")){
      String argData = request->arg("indicatorH");
      Serial.print("The indicatorH is: ");
      Serial.println(argData);

      float val = argData.toFloat();
      dtParam.highRange = val;
  } 

  else if(request->hasArg("alarmL")){
      String argData = request->arg("alarmL");
      Serial.print("The alarmL is: ");
      Serial.println(argData);

      float val = argData.toFloat();
      dtParam.lowLimit = val;
  } 

  else if(request->hasArg("alarmH")){
      String argData = request->arg("alarmH");
      Serial.print("The alarmH is: ");
      Serial.println(argData);

      float val = argData.toFloat();
      dtParam.highLimit = val;
  } 

  else if(request->hasArg("alfaEma")){
      String argData = request->arg("alfaEma");
      Serial.print("The alfaEma is: ");
      Serial.println(argData);

      float val = argData.toFloat();
      dtParam.alfaEma = val;
  } 

  else if(request->hasArg("config")){
      String argData = request->arg("config");
      Serial.print("The config is: ");
      Serial.println(argData);

      if(argData == "configT"){
        status = true;
        accessParamTemperature.setParam(dtParam);
      }

      else if(argData == "configH"){
        status = true;
        accessParamHumidity.setParam(dtParam);
      }
  } 

  else {
      Serial.println("Post did not have a 'samplingTime' field.");
  }

  return status;
}

void loadStaticFile(){
  // Route to load style.css file
  server.on("/style.css", HTTP_GET, [](AsyncWebServerRequest *request){
    request->send(LittleFS, "/style.css", "text/css");
  });

  // Route to load authentication.js file
  server.on("/authentication.js", HTTP_GET, [](AsyncWebServerRequest *request){
    request->send(LittleFS, "/authentication.js", "text/js");
  });

  // Route to load report.js file
  server.on("/report.js", HTTP_GET, [](AsyncWebServerRequest *request){
    request->send(LittleFS, "/report.js", "text/js");
  });

  // Route to load index.js file
  server.on("/index.js", HTTP_GET, [](AsyncWebServerRequest *request){
    request->send(LittleFS, "/index.js", "text/js");
  });

  // Route to load login.js file
  server.on("/login.js", HTTP_GET, [](AsyncWebServerRequest *request){
    request->send(LittleFS, "/login.js", "text/js");
  });

  // Route to load config.js file
  server.on("/config.js", HTTP_GET, [](AsyncWebServerRequest *request){
    request->send(LittleFS, "/config.js", "text/js");
  });

  // Route to load widgets.js file
  server.on("/widgets.js", HTTP_GET, [](AsyncWebServerRequest *request){
    request->send(LittleFS, "/widgets.js", "text/js");
  });

  // Route to load logoGMF file
  server.on("/logoGMF", HTTP_GET, [](AsyncWebServerRequest *request){
    request->send(LittleFS, "/logoGMF.png", "image/png");
  });

}

void listAllFilesInDir(String dir_path){
	Dir dir = LittleFS.openDir(dir_path);
	while(dir.next()) {
		if (dir.isFile()) {
			// print file names
			Serial.print("File: ");
			Serial.println(dir_path + dir.fileName());
		}
		if (dir.isDirectory()) {
			// print directory names
			Serial.print("Dir: ");
			Serial.println(dir_path + dir.fileName() + "/");
			// recursive file listing inside new directory
			listAllFilesInDir(dir_path + dir.fileName() + "/");
		}
	}
}

void loadUsers(){
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
  fullFileName.toCharArray(fileNameChar,31);

  File file = LittleFS.open(fileNameChar, "r");
  if (!file) {
    Serial.println("Failed to open file for reading");
  }
  else{
    StaticJsonDocument<384> doc;

    DeserializationError error = deserializeJson(doc, file);

    if (error) {
      Serial.print(F("deserializeJson() failed: "));
      Serial.println(error.f_str());
      return;
    }

    JsonObject Engineer = doc["Engineer"];
    //Engineer
    userDt.username = (String) Engineer["username"];
    userDt.password = (String) Engineer["password"];
    userDt.email = (String) Engineer["email"];
    String level = (String) Engineer["level"];
    userDt.level = level.toInt();
    accessEngineer.setUser(userDt);

    JsonObject Operator = doc["Operator"];
    //Engineer
    userDt.username = (String) Operator["username"];
    userDt.password = (String) Operator["password"];
    userDt.email = (String) Operator["email"];
    level = (String) Operator["level"];
    userDt.level = level.toInt();
    accessOperator.setUser(userDt);

    file.close();
  }
}

void setupDefaultUser(){
  userData userDt;
  userDt.username = "guest";
  userDt.password = "guest";
  userDt.email = "guest@example.com";
  userDt.level = 0;
  activeUser.setUser(userDt);
}


//NTP
bool getLocalTime(struct tm * info, uint32_t ms) {
  uint32_t count = ms / 10;
  time_t now;

  time(&now);
  localtime_r(&now, info);

  if (info->tm_year > (2016 - 1900)) {
    return true;
  }

  while (count--) {
    delay(10);
    time(&now);
    localtime_r(&now, info);
    if (info->tm_year > (2016 - 1900)) {
      return true;
    }
  }
  return false;
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
