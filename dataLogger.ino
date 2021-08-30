//
//  ESP8266 and the Arduino IDE Part 10d: IOT Monitor 
//  ESP8266-10D_sketch_001_Final
//


const boolean DEBUG = true;

// credentials used for AP mode
const char APssid[] = "IOT-MON";
const char APpass[] = "12345678";
const char APip[] = "192.168.4.1";
char const LCD_Title[]    = "IOT Monitor         ";  // using a 4 x 20 LCD the max length is 20 characters

#include <ESP8266WiFi.h>
#include <WebSocketsServer.h>
 
WiFiServer server(80);
WebSocketsServer webSocket = WebSocketsServer(81);
// it takes a short while for the websocket to initise fully. If you send the data too early it will be ignored.
// webSocketIsOpen is used to show the websocket is ready.
boolean webSocketIsOpen = false;

#include "DHT.h"
#define DHTPIN D5     
#define DHTTYPE DHT11   
DHT dht(DHTPIN, DHTTYPE);


#include <LiquidCrystal_I2C.h>
LiquidCrystal_I2C lcd(0x27, 20, 4);

byte const pinLDR     = A0;
byte const pinSWITCH2 = D0;   // general use momentary push button switch
byte const pinSWITCH  = D6;   // temperature scale selector. 

boolean switchPinState   = LOW;
boolean btnSwitchPrevState = HIGH;

boolean const CELSIUS = true;
boolean const FAHRENHEIT = false;
boolean whichTemp = 0;
boolean oldTemp = 0;

long sensorUpdate_Frequency = 60000;     // update the sensors values. 60000 = 60 seconds
long sensorUpdate_TimePrev = 0;
long timeNow = 0;

long displayInfo_TimeActive = 4000;      // show the network information for 4 secnds
long displayInfo_StartTime = 0;
boolean displayInfo_Active = false;


// Simple Connection Manager            // add the Simple Connection Manager file
#include "SCM.h"
 
void setup()
{
  Serial.begin(115200);
  while (!Serial) {;}
  if (DEBUG) { Serial.print(F("\n\nSerial started at 115200\n" ));   }
  
  lcd.begin(20,4);
  lcd.init();
  lcd.backlight();

  pinMode(pinSWITCH, INPUT); 
  pinMode(pinSWITCH2, INPUT);
  dht.begin();
  
  // If there are any saved credentials the ESP8266 will try to auto connect even if you haven't asked it to.
  // WiFi.disconnect() disconnects (if connected) and erases saved network credentials. 
  // It is an easy way to reset saved credentials.
  if ( digitalRead( pinSWITCH2)== LOW ) {  WiFi.disconnect();  }

  // if there is a saved SSID try to connect
  // WiFi.SSID() is the saved SSID. 
  // The ESP8266 saves the network credentials automatically when a connection is made 
  if (WiFi.SSID() != "" ) 
  {
    if (DEBUG) { Serial.print(F("Connecting to ")); Serial.print(WiFi.SSID()); Serial.print(F("  ")); }
      lcd.setCursor(0, 0);  lcd.print(LCD_Title);
      lcd.setCursor(0, 1);  lcd.print(F("Connecting to"));
      lcd.setCursor(0, 2);  lcd.print(WiFi.SSID());

      // uses AP and STA mode together
      // This means you can access the webapp via the local network or by connecting to the ESP8266 directly
      WiFi.mode(WIFI_AP_STA);
      WiFi.begin();

      lcd.setCursor(0, 3);  
      // time out after 10 seconds (500ms * 20)
      byte count = 0;   
      while ( (WiFi.status()!=WL_CONNECTED ) && ( count< 20) )
      {   
          lcd.print(F("."));   
          if (DEBUG) { Serial.print(F(".")); }
          count++;  
          delay(500);
      }
      if (DEBUG) { Serial.println(); }

      if ( WiFi.status()== WL_CONNECTED  )  
      {
          if (DEBUG) { Serial.print(F("Connected to ")); Serial.println(WiFi.SSID());    }
          if (DEBUG) { Serial.print(F("IP address: "));  Serial.println(WiFi.localIP()); }
          // debug(WiFi.localIP().toString());   
          lcd.clear();
          lcd.setCursor(0, 0);  lcd.print(LCD_Title);
          lcd.setCursor(0, 1);  lcd.print(F("Connected to"));
          lcd.setCursor(0, 2);  lcd.print( WiFi.SSID() );   // length of SSID is not checked. If it is > 20 there will be overflow.
          lcd.setCursor(0, 3);  lcd.print(F("IP: ")); lcd.print(WiFi.localIP());

          WiFi.softAP(APssid, APpass);
          delay (4000);
      }
      else
      {
          if (DEBUG) { Serial.print(F("Connection to ")); Serial.print(WiFi.SSID()); Serial.println(F( " failed\n")); }
          lcd.clear();
          lcd.setCursor(0, 0);  lcd.print(LCD_Title);
          lcd.setCursor(0, 2);  lcd.print(F("Failed to connect to"));
          lcd.setCursor(0, 3);  lcd.print( WiFi.SSID() );  // length of SSID is not checked. If it is > 20 there will be overflow.
          delay (4000);
      }

  }  //if (WiFi.SSID() != "")

  else 
  {
          if (DEBUG) { Serial.println(F("No saved SSID")); } 
          lcd.setCursor(0,0);  lcd.print(LCD_Title);
          lcd.setCursor(0,1);  lcd.print(F("No saved credentials"));
          delay(1000);
          lcd.setCursor(0,2);  lcd.print(F("Opening the"));
          lcd.setCursor(0,3);  lcd.print(F("connection page"));
          delay (2000);
  }


  // if not connected open the wifi connection page
  if ( WiFi.status()!=WL_CONNECTED )
  {
      // Simple Connection Manager return value: 
      // 0 = no networks. 1 = have networks but connection failed. 2 = connected to local network.

      if (DEBUG) { Serial.println(F("Starting SCM")); }
      long timeOut = 60000; // Set the SCM time out to 60 seconds
      byte connectionResult = simpleConnectionManager(APssid, APpass, timeOut);

      if (DEBUG) { Serial.print(F("connectionResult = ")); Serial.println(connectionResult);  } 
      if ( connectionResult ==0  ||  connectionResult ==1 ) // no networks found or not connected
      {  
           lcd.clear();
           lcd.setCursor(0, 0);
           if ( connectionResult ==0 )  { lcd.print(F("No networks found"));  if(DEBUG) { Serial.println(F("No networks found")); } }
           if ( connectionResult ==1 )  { lcd.print(F("Connection failed"));  if(DEBUG) { Serial.print(F("Cannot connect to "));  Serial.println( WiFi.SSID() );    }  }

            lcd.setCursor(0, 1);  lcd.print(F("Connect directly"));
            lcd.setCursor(0, 2);  lcd.print(APssid);
            lcd.setCursor(0, 3);  lcd.print(APip);
            WiFi.softAP(APssid,APpass); 
            delay(4000);
      } 

      else if ( connectionResult ==2 ) // Connection successful
      {
            lcd.clear();
            lcd.setCursor(0, 0);  lcd.print(LCD_Title);
            lcd.setCursor(0, 1);  lcd.print(F("Connected to:"));
            lcd.setCursor(0, 2);  lcd.print( WiFi.SSID() );  //  length of the SSID name is not checked
            lcd.setCursor(0, 3);  lcd.print(F("IP: "));  lcd.print(WiFi.localIP() );

            if (DEBUG) { Serial.print(F("Connected to ")); Serial.print(WiFi.SSID()); }
            if (DEBUG) { Serial.print(F(",  IP address: ")); Serial.println(WiFi.localIP()); }
           //debug(F(",  IP address: "));   debug(WiFi.localIP().toString() ); debug(F("\n")); 
      }

        
        
  }  //  if ( WiFi.status()!=WL_CONNECTED )


  // pinSWITCH is used to determine the temperature scale C or F
  switchPinState = digitalRead(pinSWITCH);
  if (switchPinState==HIGH) {whichTemp = "F"; } else {whichTemp = "C";}
  oldTemp = whichTemp;
    
        
  // start a server
  server.begin();
  if (DEBUG) { Serial.println(F("Server started\n")); }

  // start the websocket
  webSocket.begin();
  // webSocketEvent is the function to call when there is a websocket event
  webSocket.onEvent(webSocketEvent);
  if (DEBUG) { Serial.println(F("websocket initialised\n")); }

  // show the sensor values on the LCD. false means LCD only, do not update the webpage
  // there is no point sending websocket data until the website is running.
  updateSensors(webSocketIsOpen);

} // setup()



 
 
void loop()
{
    
    // check the websocket
    webSocket.loop();

    // Check if a client has connected
    WiFiClient client = server.available();     
   
    // if client is true then we have a request
    if (client)  
    {   
      String REQUEST = client.readStringUntil('\r');
      if (DEBUG) { Serial.print(F("\nRequest = "));  Serial.println(REQUEST);  } 

      int URI_START = REQUEST.indexOf(" ");
      int URI_END   = REQUEST.indexOf(" ", URI_START+1 );

      if (URI_START != -1 && URI_END != -1)
      { 
        String URI    = REQUEST.substring(URI_START+1, URI_END);
        URI.toLowerCase();
        if (DEBUG) { Serial.print(F("URI = "));   Serial.println(URI);  }

        // IOT Monitor only has one page so we are only interested in root. Anything else can be ignored.
        if  ( URI == "/" )  
        {
            client.flush();
            client.print( HTTP_HEADER );
            client.print( HTML_1 ); 
            if (DEBUG) { Serial.println(F("Page served\n")); } 
            // a new page means a new ws service. Reset the ws flag and wait for the ws start message
            webSocketIsOpen = false;
        }
      }
    }  

  
    // display the network credentials on the LCD
    // after the boot process is finished the push button switch on D0 is used to show the connection details
    // if connected to a local network show the connection details
    // if not connected to a local network show the AP mode details.
    boolean btnSwitchState = digitalRead( pinSWITCH2);
    if ( btnSwitchState == LOW )  
    {   
      if (btnSwitchPrevState != LOW)
      {
          if(DEBUG) { Serial.println(F("Show connection details")); }
          displayInfo_StartTime = millis();
          displayInfo_Active = true;
          
          btnSwitchPrevState = LOW;
          lcd.clear();
          if ( WiFi.status()== WL_CONNECTED  ) 
          {
            lcd.setCursor(0, 0);  lcd.print(LCD_Title);
            lcd.setCursor(0, 1);  lcd.print(F("Connected to"));
            lcd.setCursor(0, 2);  lcd.print( WiFi.SSID() );  // length of SSID is not checked
            lcd.setCursor(0, 3);  lcd.print(WiFi.localIP());
          }
          else
          {
            lcd.setCursor(0, 0);  lcd.print(LCD_Title);
            lcd.setCursor(0, 1);  lcd.print(F("Connect directly"));
            lcd.setCursor(0, 2);  lcd.print(APssid);
            lcd.setCursor(0, 3);  lcd.print(APip);
          }
      }
    } 
    else  { btnSwitchPrevState = HIGH; }


    // timer to return to sensor data display
    if ( displayInfo_Active )
    {
       if (millis() - displayInfo_StartTime  > displayInfo_TimeActive) 
       {
            updateSensors(false); // false = update the LCD only.
            displayInfo_Active = false;
       } 
    }



    //check the TempScaleSwitch. See if the user has changed the scale
    // if the scale has just changed then update the sensors straight away
    switchPinState = digitalRead(pinSWITCH);
    if (switchPinState==FAHRENHEIT) {whichTemp = FAHRENHEIT;} else {whichTemp = CELSIUS;}
    if (whichTemp != oldTemp) 
    {
        if (DEBUG) 
        { 
           Serial.print(F("Scale changed to ")); 
           if (whichTemp == CELSIUS) {Serial.println(F("CELSIUS"));  } else {Serial.println(F("FAHRENHEIT"));  } 
        } 
        oldTemp = whichTemp;   
        updateSensors(webSocketIsOpen); 
    }


    // timeNow is used to determin if it is time to update the sensor values.
    timeNow = millis();

    // see if it is time to update the sensor values
    if (timeNow - sensorUpdate_TimePrev > sensorUpdate_Frequency) 
    { 
        // webSocketIsOpen is either true or false
        // updateSensors(false) means update the LCD only
        // updateSensors(true)  means update the webpage and the LCD
        displayInfo_Active = false;         // reset the display net credentials timer. Just in case it is ruuning.
        updateSensors(webSocketIsOpen);     // true = update the website as well.
    }



}  // void loop()



boolean updateSensors(boolean upDateWebPage)
{
    sensorUpdate_TimePrev = timeNow;
    
    boolean reply = true;
    int brightness = map(analogRead(pinLDR), 0, 1023, 0, 12);  // brightness is a value from 0 to 12
    float humidity =   dht.readHumidity();        
    float tempC =      dht.readTemperature();                  // Read temperature as Celsius 
    float tempF =      dht.readTemperature(true);              // Read temperature as Fahrenheit (isFahrenheit = true)

    //if any value is isnan (not a number) then there is an error
    if (isnan(humidity) || isnan(tempC) || isnan(tempF))        
    {  
        if (DEBUG) { Serial.println(F("Error reading from the sensor"));     }
        // update the LCD
        lcd.clear();
        lcd.setCursor(0, 0);  lcd.print(LCD_Title);
        lcd.setCursor(0, 2);  lcd.print(F("Error reading the"));
        lcd.setCursor(0, 3);  lcd.print(F("sensor"));
        reply = false;
    }
    else // the values are OK
    {
        // send the data via websocket and update the LCD
        String data = "";
        data = String(data + byte(humidity) ); 
        data = String(data + "|"); 
        data = String(data + tempC); 
        data = String(data + "|"); 
        data = String(data + tempF); 
        data = String(data + "|"); 
        data = String(data + brightness); 
        data = String(data + "|"); 
        if (whichTemp == CELSIUS) { data = String(data + "C");   }
        else                      { data = String(data + "F");   }

        if (DEBUG) { Serial.println(data); }
        if ( upDateWebPage && webSocketIsOpen)   { webSocket.broadcastTXT(data); }
        
        // update the LCD
        lcd.clear();
        lcd.setCursor(0, 0);  lcd.print(LCD_Title);
        lcd.setCursor(0, 1);  lcd.print(F("Temperature:"));
        lcd.setCursor(0, 2);  lcd.print(F("Humidity: "));
        lcd.setCursor(0, 3);  lcd.print(F("Brightness:"));

        if ( whichTemp == CELSIUS ) 
        {  lcd.setCursor(13, 1); lcd.print(tempC); lcd.write(223);  lcd.print("C");   }
        else                          
        { 
           if ( tempF < 100 ) { lcd.setCursor(13, 1); } else { lcd.setCursor(12, 1); }
           lcd.print(tempF);  lcd.write(223);  lcd.print("F"); 
        }
        lcd.setCursor(13, 2); lcd.print(humidity);   lcd.print("%");  
        lcd.setCursor(13, 3); lcd.print(brightness);
    }
    
    return reply;
} // void updateSensors()



// webSocketEvent is called when there is a websocket event
// there are various types of event. The one used here is text and used to show the websocket have started
void webSocketEvent(byte num, WStype_t type, uint8_t * payload, size_t length)
{
    if (DEBUG) { Serial.print(F("WebSocket Event, WStype = ")); Serial.println(type); } 

    if(type == WStype_TEXT)
    {
      if (payload[0] == '#')  // the websocket on the webpage has started
      {
        if (DEBUG) { Serial.println(F("websocket started\n")); }        
        webSocketIsOpen = true;
        updateSensors(webSocketIsOpen);
      }
    }
    
}












   















 
