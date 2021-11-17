#ifndef dataLogger_h
#define dataLogger_h

#define DHTPIN 2 // what pin we're connected to ????

#define DHTTYPE DHT11
#define OLED_RESET 0 // GPIO0

const boolean DEBUG = true;

// Replace with your network credentials
const char *SSID1 = "Sam-Laptop";
const char *PASSWORD1 = "sampon170466";
const char *SSID2 = "SamMobile";
const char *PASSWORD2 = "sampon170466";
const char *SSID3 = "SamFamily";
const char *PASSWORD3 = "muwa7x06";
const char *SSID4 = "Fariyo Ganteng";
const char *PASSWORD4 = "Faryganteng";

bool AP_MODE = false; // Do yo want the ESP as AP?

const String PATH_ROOT = "/";
const String FILE_USER = "users.cfg";

//NTP : Network Time Protokol
const long timezone = 6; //for jakarta time
const byte daysavetime = 1;

// Set LED GPIO
const int ledPin = LED_BUILTIN;

const int ADC_MAX = 1023;        //10 bits
const int DEBOUNCING_TIME = 150; //milli second

const int SAMPLING_TIME_DEFAULT = 10000; //milli second, default value

//DHT22 min sampling
const int SAMPLING_TIME_MIN = 2000;  //milli second, minimum value
const int SAMPLING_TIME_MAX = 60000; //milli second, maximum value

#endif
