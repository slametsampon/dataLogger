/*
  StartUp.h - Library untuk Sequence Timer
  Dibuat Oleh : sam Sep 2017
*/
#ifndef startUp_h
#define startUp_h

    #include <Arduino.h>
    #include <Wire.h>  // Include Wire if you're using I2C
    #include <SPI.h>
    #include <Adafruit_SSD1306.h>

    #define OLED_RESET 0  // GPIO0

    enum msgIndex{
    gmf, AeroAsia, IoT, dataLogger, version, salman, alfarisi, year
    };

    String welcomeMsg[year+1] = {"GMF","AeroAsia","IoT", "DataLogger", "Ver: ", "Salman", "Alfarisi", "Oct,2021"} ;
    const String VERSION = "01.00";
    const int STEP_WAIT_TIME = 1000;//milli second, maximum value

    //logo gmf
    // 'gmf', 64x48px
    const unsigned char logo_gmf [] PROGMEM = {
        0x00, 0x7f, 0xc0, 0x00, 0x00, 0x00, 0x00, 0x00, 0x40, 0x00, 0x7f, 0xc0, 0x00, 0x00, 0x00, 0x00, 
        0x00, 0x00, 0x03, 0xf8, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xfe, 0x00, 0x00, 0x00, 0x00, 
        0x00, 0x00, 0x00, 0x3f, 0x80, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x0f, 0xc0, 0x00, 0x00, 0x00, 
        0x00, 0x00, 0x00, 0x07, 0xe0, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x03, 0xf0, 0x00, 0x00, 0x00, 
        0x1f, 0xff, 0xfe, 0x01, 0xf8, 0x00, 0x00, 0x00, 0x00, 0x00, 0x0f, 0xe0, 0xfc, 0x00, 0x00, 0x00, 
        0x00, 0x00, 0x01, 0xf8, 0x7e, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x7e, 0x3f, 0x00, 0x00, 0x00, 
        0x00, 0x00, 0x00, 0x1f, 0x9f, 0x80, 0x00, 0x00, 0x00, 0x00, 0x00, 0x0f, 0xcf, 0xc0, 0x00, 0x00, 
        0x00, 0x00, 0x00, 0x07, 0xe7, 0xe0, 0x00, 0x00, 0x03, 0xff, 0xe0, 0x03, 0xf3, 0xf2, 0x00, 0x00, 
        0x00, 0x00, 0x7f, 0x81, 0xf9, 0xfb, 0xff, 0x80, 0x00, 0x00, 0x03, 0xe0, 0xfc, 0xf9, 0xff, 0xc0, 
        0x00, 0x00, 0x00, 0xfc, 0x7e, 0xfc, 0xfe, 0x00, 0x00, 0x00, 0x00, 0x3e, 0x3f, 0x7e, 0x7e, 0xf8, 
        0x00, 0x00, 0x00, 0x1f, 0x9f, 0xbf, 0x3f, 0xfc, 0x00, 0x00, 0x00, 0x0f, 0xcf, 0xdf, 0x9f, 0xfe, 
        0x00, 0x00, 0x00, 0x07, 0xe7, 0xcf, 0xcf, 0xfe, 0x00, 0x7f, 0xfe, 0x03, 0xf7, 0xe7, 0xe7, 0x82, 
        0x00, 0x00, 0x0f, 0xc1, 0xfb, 0xf3, 0xf7, 0x00, 0x00, 0x00, 0x03, 0xf0, 0xfd, 0xf9, 0xf8, 0x00, 
        0x00, 0x00, 0x00, 0xfc, 0x7e, 0xfc, 0xfc, 0x00, 0x00, 0x00, 0x00, 0x3f, 0x3e, 0x7e, 0x78, 0x00, 
        0x00, 0x00, 0x00, 0x1f, 0x9f, 0x3f, 0x30, 0x00, 0x00, 0x00, 0x00, 0x0f, 0xdf, 0x9f, 0x80, 0x00, 
        0x00, 0x0f, 0xf0, 0x07, 0xef, 0xcf, 0xc0, 0x00, 0x00, 0x00, 0x1f, 0x83, 0xf7, 0xe7, 0xc0, 0x00, 
        0x00, 0x00, 0x03, 0xe1, 0xfb, 0xf3, 0x80, 0x00, 0x00, 0x00, 0x00, 0xf8, 0xf9, 0xf8, 0x00, 0x00, 
        0x00, 0x00, 0x00, 0x7c, 0xfc, 0xfc, 0x00, 0x00, 0x00, 0x00, 0x00, 0x3f, 0x7e, 0x7c, 0x00, 0x00, 
        0x00, 0x00, 0x00, 0x1f, 0xbf, 0x3c, 0x00, 0x00, 0x00, 0x00, 0x00, 0x0f, 0xdf, 0x90, 0x00, 0x00, 
        0x00, 0x00, 0x00, 0x07, 0xcf, 0xc0, 0x00, 0x00, 0x00, 0x00, 0x00, 0x03, 0xe7, 0xe0, 0x00, 0x00, 
        0x00, 0x00, 0x00, 0x01, 0xf3, 0xc0, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01, 0xf9, 0x80, 0x00, 0x00, 
        0x00, 0x00, 0x00, 0x00, 0xfc, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x7e, 0x00, 0x00, 0x00, 
        0x00, 0x00, 0x00, 0x00, 0x3e, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x1c, 0x00, 0x00, 0x00, 
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00
    };

    class StartUp{
    public:
        StartUp(String );      
        void AttachDisplay(Adafruit_SSD1306*);
        void logoDisplay();//display logo GMF
        void welcomeDisplay();//welcome display
        void stepDisplay(int);//display step of setup

    private:
        String _id;
        Adafruit_SSD1306 *_display;
    };

#endif

StartUp::StartUp(String id): _id(id){}

void StartUp::AttachDisplay(Adafruit_SSD1306 *display){
    _display = display;
}

void StartUp::logoDisplay(){

    //display logo gmf - bitmap
    _display->clearDisplay();
	_display->drawBitmap(0, 0, logo_gmf, 64, 48, WHITE);
	_display->display();
    delay(3000);
}

void StartUp::welcomeDisplay(){
    int x,y, totalH;
    //for oled 64x48
    int oledWidth = 64;
    int oledHeight = 48;

    int maxLine = 6;//for texSize 1;
    int maxCol = 10;//for texSize 1

    int hight = oledHeight/maxLine;

    String msg;

    //welcomeMsg [year+1] = {"GMF","AeroAsia","IoT", "DataLogger", "Ver: ", "Salman", "Alfarisi", "Oct,2021"} ;
    for (int msgNbr = 0; msgNbr <= year; msgNbr++) {
        //y = line * hight;

        //Show GMF AeroAsia
        //clear display
        if(msgNbr == gmf){
            _display->clearDisplay();

            //Characters are rendered in the ratio of 7:10. 
            //Meaning, passing font size 1 will render the text at 7×10 pixels per character, 
            //passing 2 will render the text at 14×20 pixels per character and so on.
            _display->setTextSize(1);
            _display->setTextColor(WHITE);

            totalH = 2 * 8;
            y = (oledHeight - totalH)/2 + 4/2 - 4;
            x = (maxCol - 3) * 6/2; 
            _display->setCursor(x,y);//x,y on pixel
            msg = welcomeMsg[msgNbr];
            _display->print(msg);
        }

        //show to oled display
        else if(msgNbr == AeroAsia){
            // Show the display buffer on the hardware.
            // NOTE: You _must_ call display after making any drawing commands
            // to make them visible on the display hardware!
            //AeroAsia y = 14 + 8 + 4 = 26
            y += 14;
            x = (maxCol - 9) * 6/2; 
            _display->setCursor(x,y);//x,y on pixel
            msg = welcomeMsg[msgNbr];
            _display->print(msg);

            _display->display();
            delay (2000);
        }

        //Show IoT, DataLogger, Version
        //clear display
        else if(msgNbr == IoT){
            _display->clearDisplay();

            //Characters are rendered in the ratio of 7:10. 
            //Meaning, passing font size 1 will render the text at 7×10 pixels per character, 
            //passing 2 will render the text at 14×20 pixels per character and so on.
            _display->setTextSize(1);
            _display->setTextColor(WHITE);

            totalH = 3 * 8;
            y = (oledHeight - totalH)/2 - 4;
            x = (maxCol - 3) * 6/2; 
            _display->setCursor(x,y);//x,y on pixel
            msg = welcomeMsg[msgNbr];
            _display->print(msg);
        }

        else if(msgNbr == dataLogger){
            y += 14;
            x = (maxCol - 10) * 6/2; 
            _display->setCursor(x,y);//x,y on pixel
            msg = welcomeMsg[msgNbr];
            _display->print(msg);
        }

        else if(msgNbr == version){
            y += 14;
            x = (maxCol - 10) * 6/2; 
            _display->setCursor(x,y);//x,y on pixel
            msg = welcomeMsg[msgNbr];
            _display->print(msg);
            _display->print(VERSION);

            _display->display();
            delay (2000);
        }

        //Show Salman, Alfarisi, year
        //clear display
        else if(msgNbr == salman){
            _display->clearDisplay();

            //Characters are rendered in the ratio of 7:10. 
            //Meaning, passing font size 1 will render the text at 7×10 pixels per character, 
            //passing 2 will render the text at 14×20 pixels per character and so on.
            _display->setTextSize(1);
            _display->setTextColor(WHITE);

            totalH = 3 * 8;
            y = (oledHeight - totalH)/2 - 4;
            x = (maxCol - 6) * 6/2; 
            _display->setCursor(x,y);//x,y on pixel
            msg = welcomeMsg[msgNbr];
            _display->print(msg);
        }

        else if(msgNbr == alfarisi){
            y += 14;
            x = (maxCol - 7) * 6/2; 
            _display->setCursor(x,y);//x,y on pixel
            msg = welcomeMsg[msgNbr];
            _display->print(msg);
        }

        //Oct,2021
        else if(msgNbr == year){
            y += 14;
            x = (maxCol - 7) * 6/2; 
            _display->setCursor(x,y);//x,y on pixel
            msg = welcomeMsg[msgNbr];
            _display->print(msg);

            _display->display();
            delay (2000);
        }
    }
}

void StartUp::stepDisplay(int step){

    //convert to string
    String stepStr = String(step);
    if (step<10) stepStr = "0" + String(step);

    _display->clearDisplay();
    _display->setTextSize(2);
    _display->setTextColor(WHITE);

    _display->setCursor(8,12);//x,y on pixel
    _display->print("STEP");
    _display->setCursor(20,28);//x,y on pixel
    _display->print(stepStr);

    _display->display();
    delay (STEP_WAIT_TIME);
}
