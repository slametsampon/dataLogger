### A. dataLogger.ino
#### 1. Declaration :
####
1. All users (AccesUser accessEngineer("accessEngineer");
AccesUser accessOperator("accessOperator");
AccesUser activeUser("activeUser");)

2. SequenceTimer   mainSequence("mainSequence");

3. AccessParam accessParamTemperature("accessParamTemperature");
AccessParam accessParamHumidity("accessParamHumidity");

4. Adafruit_SSD1306 display(OLED_RESET);
DHT dht(DHTPIN, DHTTYPE);
Logsheet logsheet("logsheet");

5. ESP8266WiFiMulti wifiMulti;
6. AsyncWebServer server(80);

#### 2. Setup - STEP:
#####
1. Serial
2. Initialize LittleFS
3. listAllFilesInDir in LittleFS
4. setup samplingTime to default value
5. load Engineer and Operator from littleFS
6. setup active user to default (guest)
7. setup pin
8. Initialize loghseet (
  logsheet.AttachParameter(&accessParamTemperature, &accessParamHumidity);
  logsheet.AttachSensor(&dht);
  logsheet.AttachDisplay(&display);
  logsheet.info();)
9. Start WiFi
  (if (WiFiAP) startWiFiAP();
  //else startWiFiClient();
  else startWiFiMulti();)
10. Get time from NTP Server
11. logsheet setTime - initial
12. start mDNS
13. urlController();
14. Start server

#### 3. Loop
####
1. Execute logsheet with sampling time
2. Run mainSequence.execute();
3. Update logsheet time - per minute
