/*
  Rui Santos
  Complete project details at https://RandomNerdTutorials.com
  
  Permission is hereby granted, free of charge, to any person obtaining a copy
  of this software and associated documentation files.
  
  The above copyright notice and this permission notice shall be included in all
  copies or substantial portions of the Software.
*/

// Import required libraries
#include <ESP8266WiFi.h>
#include <ESPAsyncTCP.h>
#include <ESPAsyncWebServer.h>
#include <WebSocketsServer.h>

#include <LittleFS.h>
#include "DHT.h"

#define DHTPIN D5     
#define DHTTYPE DHT11   

const boolean DEBUG = true;

// Replace with your network credentials
const char* ssid     = "Sam-Laptop";
const char* password = "sampon170466";

// Set LED GPIO
const int ledPin = LED_BUILTIN;
// Stores LED state
String ledState;

DHT dht(DHTPIN, DHTTYPE);

// Create AsyncWebServer object on port 80
AsyncWebServer server(80);

WebSocketsServer webSocket = WebSocketsServer(81);
// it takes a short while for the websocket to initise fully. If you send the data too early it will be ignored.
// webSocketIsOpen is used to show the websocket is ready.
boolean webSocketIsOpen = false;

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
 
void setup(){
  // Serial port for debugging purposes
  Serial.begin(115200);
  while (!Serial) {;}
  if (DEBUG) { Serial.print(F("\n\nSerial started at 115200\n" ));   }

  pinMode(ledPin, OUTPUT);

  // Initialize the sensor

  // Initialize LittleFS
  if(!LittleFS.begin()){
    Serial.println("An Error has occurred while mounting LittleFS");
    return;
  }

  // Connect to Wi-Fi
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.println("Connecting to WiFi..");
  }

  // Print ESP32 Local IP Address
  Serial.println(WiFi.localIP());

  // Route for root / web page
  server.on("/", HTTP_GET, [](AsyncWebServerRequest *request){
    request->send(LittleFS, "/index.html", String(), false, processor);
  });
  
  server.on("/report", HTTP_GET, [](AsyncWebServerRequest *request){
    request->send(LittleFS, "/report.html", String(), false, processor);
  });
  
  // Route to load style.css file
  server.on("/style.css", HTTP_GET, [](AsyncWebServerRequest *request){
    request->send(LittleFS, "/style.css", "text/css");
  });

  // Route to load script_functions.js file
  server.on("/script_functions.js", HTTP_GET, [](AsyncWebServerRequest *request){
    request->send(LittleFS, "/script_functions.js", "text/js");
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

  server.on("/temperature", HTTP_GET, [](AsyncWebServerRequest *request){
    request->send_P(200, "text/plain", getTemperature().c_str());
  });
  
  server.on("/humidity", HTTP_GET, [](AsyncWebServerRequest *request){
    request->send_P(200, "text/plain", getHumidity().c_str());
  });
  
  // Start server
  server.begin();
}
 
void loop(){
  
}
