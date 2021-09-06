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

#include <LittleFS.h>

#include "model.h"
#include "dataLogger.h"
#include "SequenceTimer.h"
#include "logsheet.h"

SequenceTimer   mainSequence("Sequence");

AccessParam accessParamTemperature("accessParamTemperature");
AccessParam accessParamHumidity("accessParamHumidity");

DHT dht(DHTPIN, DHTTYPE);
Adafruit_SSD1306 display(OLED_RESET);

Logsheet logsheetTask("logsheetTask");

// Stores LED state
String ledState;

// Create AsyncWebServer object on port 80
AsyncWebServer server(80);

WebSocketsServer webSocket = WebSocketsServer(81);
// it takes a short while for the websocket to initise fully. If you send the data too early it will be ignored.
// webSocketIsOpen is used to show the websocket is ready.
boolean webSocketIsOpen = false;

//functions prototype
void startWiFiAP();
void startWiFiClient();
void startMDNS();
void urlController();

//LittleFS file operations
//https://circuitdigest.com/microcontroller-projects/littlefs-with-esp8266-to-read-write-and-delete-data-on-flash-memory-of-nodemcu
void readData();
void writeData(String data);
void deleteData();

void setup(){
  // Serial port for debugging purposes
  Serial.begin(115200);
  while (!Serial) {;}
  if (DEBUG) { Serial.print(F("\n\nSerial started at 115200\n" ));   }

  pinMode(ledPin, OUTPUT);

  // Initialize the sensor
  //setupParameter();
  logsheetTask.AttachSensor(&dht);
  logsheetTask.info();

  // Initialize LittleFS
  Serial.println("Begin LittleFS");
  if(!LittleFS.begin()){
    Serial.println("An Error has occurred while mounting LittleFS");
    return;
  }

  // Start WiFi
  if (WiFiAP)
    startWiFiAP();
  else
    startWiFiClient();

  Serial.println("Contacting Time Server");
  configTime(3600 * timezone, daysavetime * 3600, "time.nist.gov", "0.pool.ntp.org", "1.pool.ntp.org");
  struct tm tmstruct ;
  delay(2000);
  tmstruct.tm_year = 0;
  getLocalTime(&tmstruct, 5000);
  Serial.printf("\nNow is : %d-%02d-%02d %02d:%02d:%02d\n", (tmstruct.tm_year) + 1900, (tmstruct.tm_mon) + 1, tmstruct.tm_mday, tmstruct.tm_hour, tmstruct.tm_min, tmstruct.tm_sec);
  Serial.println("");
  
  //start mDNS
  startMDNS();

  //url controller
  urlController();

  // Start server
  server.begin();
}
 
void loop(){
  
}

void startWiFiClient(){
  Serial.println("Connecting to "+(String)ssid);
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);
  
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
  WiFi.softAP(ssid, password);
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

  // Route to load style.css file
  server.on("/style.css", HTTP_GET, [](AsyncWebServerRequest *request){
    request->send(LittleFS, "/style.css", "text/css");
  });

  // Route to load report.js file
  server.on("/report.js", HTTP_GET, [](AsyncWebServerRequest *request){
    request->send(LittleFS, "/report.js", "text/js");
  });

  // Route to load functions.js file
  server.on("/functions.js", HTTP_GET, [](AsyncWebServerRequest *request){
    request->send(LittleFS, "/functions.js", "text/js");
  });

  // Route to load widgets.js file
  server.on("/widgets.js", HTTP_GET, [](AsyncWebServerRequest *request){
    request->send(LittleFS, "/widgets.js", "text/js");
  });

  // Route for root / web page
  server.on("/", HTTP_GET, [](AsyncWebServerRequest *request){
    request->send(LittleFS, "/index.html", String(), false, processor);
  });
  
  server.on("/report", HTTP_GET, [](AsyncWebServerRequest *request){
    request->send(LittleFS, "/report.html", "text/html");
  });
  
  server.on("/jsonData", HTTP_GET, [](AsyncWebServerRequest *request){
    String webPage = initRandomJson();
    request->send(200, "application/json", webPage);
  });
  
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

  // route to update sensor - temperature and humidity
  server.on("/getSensor", HTTP_GET, [](AsyncWebServerRequest *request){
    String strDhtVal = logsheetTask.getValues();
    request->send(200, "application/json", strDhtVal);
    Serial.println(strDhtVal);
  });
  
    server.on("/login", HTTP_GET, [](AsyncWebServerRequest * request){
    request->send(LittleFS, "/login.html", "text/html");
  });

  server.on("/login", HTTP_ANY, [](AsyncWebServerRequest * request){
    if(request->hasArg("username")){
        String arg = request->arg("username");
        Serial.print("The username is: ");
        Serial.println(arg);
        request->send(LittleFS, "/index.html", "text/html");
    } else {
        Serial.println("Post did not have a 'username' field.");
    }
  });

  server.on("/config", HTTP_GET, [](AsyncWebServerRequest *request){
    request->send(LittleFS, "/config.html", "text/html");
  });
  
  server.on("/config", HTTP_ANY, [](AsyncWebServerRequest * request){
    if(request->hasArg("samplingTime")){
        String arg = request->arg("samplingTime");
        Serial.print("The samplingTime is: ");
        Serial.println(arg);
        request->send(LittleFS, "/config.html", "text/html");
    } else {
        Serial.println("Post did not have a 'samplingTime' field.");
    }
  });

/*  
  server.on("/temperature", HTTP_GET, [](AsyncWebServerRequest *request){
    request->send_P(200, "text/plain", getTemperature().c_str());
  });
  
  server.on("/humidity", HTTP_GET, [](AsyncWebServerRequest *request){
    request->send_P(200, "text/plain", getHumidity().c_str());
  });

*/
}

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

void handleLogin(AsyncWebServerRequest *request){
/*
  const char* PARAM_KEY = "key";
  String keyVal;
  if (request->hasParam(PARAM_KEY)) {
    keyVal= request->getParam(PARAM_KEY)->value();
  } else {
    //handle an incomplete request
    keyVal = "some default value";
  }
*/
}

void handleConfig(AsyncWebServerRequest *request){

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
 
String initRandomJson(){
  String output;
  DynamicJsonDocument doc(1536);
  float h, t;
  doc.clear();
  JsonArray time = doc.createNestedArray("time");
  JsonArray temperature = doc.createNestedArray("temperature");
  JsonArray humidity = doc.createNestedArray("humidity");

  for (int i=0; i < 24; i++){
    h = random(400.0, 950)/10.0;
    t = random(200.0, 455)/10.0;

    time.add(i);
    temperature.add(t);
    humidity.add(h);
  } 

  serializeJson(doc, output);
  return output;
}

//LittleFS - Operations
void readFile(const char * path) {
  Serial.printf("Reading file: %s\n", path);

  File file = LittleFS.open(path, "r");
  if (!file) {
    Serial.println("Failed to open file for reading");
    return;
  }

  Serial.print("Read from file: ");
  while (file.available()) {
    Serial.write(file.read());
  }
  file.close();
}

void writeFile(const char * path, const char * message) {
  Serial.printf("Writing file: %s\n", path);

  File file = LittleFS.open(path, "w");
  if (!file) {
    Serial.println("Failed to open file for writing");
    return;
  }
  if (file.print(message)) {
    Serial.println("File written");
  } else {
    Serial.println("Write failed");
  }
  delay(2000); // Make sure the CREATE and LASTWRITE times are different
  file.close();
}

void appendFile(const char * path, const char * message) {
  Serial.printf("Appending to file: %s\n", path);

  File file = LittleFS.open(path, "a");
  if (!file) {
    Serial.println("Failed to open file for appending");
    return;
  }
  if (file.print(message)) {
    Serial.println("Message appended");
  } else {
    Serial.println("Append failed");
  }
  file.close();
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
