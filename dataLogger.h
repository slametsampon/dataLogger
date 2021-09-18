#ifndef dataLogger_h
#define dataLogger_h

    // #define DHTPIN D5     // ????
    #define DHTPIN D4     // what pin we're connected to ????

    #define DHTTYPE DHT11   
    #define OLED_RESET 0  // GPIO0
    

    const boolean DEBUG = true;

    // Replace with your network credentials
    const char* ssid1     = "Sam-Laptop";
    const char* password1 = "sampon170466";
    const char* ssid2     = "SamMobile";
    const char* password2 = "sampon170466";
    const char* ssid3     = "SamFamily";
    const char* password3 = "muwa7x06";
    const char* ssid4     = "Fariyo Ganteng";
    const char* password4 = "Fariyoganteng";

    bool WiFiAP = false;      // Do yo want the ESP as AP?

    #define SMTP_HOST "smtp.gmail.com"
    #define SMTP_PORT 465

    /* The sign in credentials */
    #define AUTHOR_EMAIL "alumni86smansagtg@gmail.com"
    #define AUTHOR_PASSWORD "alumiSmansa86gtg"

    /* Recipient's email*/
    #define RECIPIENT_EMAIL "slametsambwi@gmail.com"

    const char* mdnsName = "dataLogger";

    const String PATH_ROOT = "/";
    const String FILE_USER = "users.cfg";

    //NTP : Network Time Protokol
    const long timezone = 6;//for jakarta time
    const byte daysavetime = 1;

    // Set LED GPIO
    const int ledPin = LED_BUILTIN;

    const int ADC_MAX = 1023;//10 bits
    const int DEBOUNCING_TIME = 150;//milli second

    const int BLINK_NORMAL = 500;//milli second
    const int BLINK_WARNING = 250;//milli second
    const int BLINK_ERROR = 125;//milli second
    const int SAMPLING_TIME = 10000;//milli second

#endif
