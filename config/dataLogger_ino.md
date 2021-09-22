### A. dataLogger.ino
#### 1. Declaration :
####
1. led 
2. StarUp
3. All users (AccesUser accessEngineer("accessEngineer");
AccesUser accessOperator("accessOperator");
AccesUser activeUser("activeUser");)

4. SequenceTimer   mainSequence("mainSequence");

5. AccessParam accessParamTemperature("accessParamTemperature");
AccessParam accessParamHumidity("accessParamHumidity");

6. Adafruit_SSD1306 display(OLED_RESET);
DHT dht(DHTPIN, DHTTYPE);
Logsheet logsheet("logsheet");

7. ESP8266WiFiMulti wifiMulti;
8. AsyncWebServer server(80);

#### 2. Setup - STEP:
#####
1. Initialize OLED, display logo & welcome
2. Initialize pin and serial comm
3. Initialize LittleFS & list directories and files
4. Load Engineer, Operator and default active users from littleFS
5. Initialize loghseet:(Initialize dhtSensor,
  logsheet.AttachParameter(&accessParamTemperature, &accessParamHumidity);
  logsheet.AttachSensor(&dht);
  logsheet.AttachDisplay(&display);
  logsheet.info(); setTime - initial)
6. Start WiFi
  (if (WiFiAP) startWiFiAP();
  //else startWiFiClient();
  else startWiFiMulti();)
7. start mDNS
8. urlController() include static files;
9. Start server
10. Get time from NTP Server

#### 3. Loop
####
1. Execute logsheet with sampling time
2. led.blink(normal)
3. Run mainSequence.execute();
4. Update logsheet time - per minute
