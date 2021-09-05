// #define DHTPIN D5     // ????
#define DHTPIN D4     // what pin we're connected to ????

#define DHTTYPE DHT11   
#define OLED_RESET 0  // GPIO0
 

const boolean DEBUG = true;

// Replace with your network credentials
//const char* ssid     = "Sam-Laptop";
const char* ssid     = "SamMobile";
const char* password = "sampon170466";
const char* mdnsName = "dataLogger";

//NTP : Network Time Protokol
const long timezone = 6;//for jakarta time
const byte daysavetime = 1;

// Set LED GPIO
const int ledPin = LED_BUILTIN;

bool WiFiAP = false;      // Do yo want the ESP as AP?

const int ADC_MAX = 1023;//10 bits
const int ALFA_EMA = 80;//AlfaEma filter
const int DEBOUNCING_TIME = 150;//milli second

const int BLINK_NORMAL = 500;//milli second
const int BLINK_WARNING = 250;//milli second
const int BLINK_ERROR = 125;//milli second

