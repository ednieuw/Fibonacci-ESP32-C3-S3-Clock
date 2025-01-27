/* 

--------------------------------------  
 Author .    : Ed Nieuwenhuys
 Changes V001: Derived from ESP32Arduino_WordClockV013. Compiling for C3
 Changes V002: No compile errors
 Changes V003: Cleaned code
 Changes V004: Stable version for ESP32C3_SUPERMINI
 Changes V005: Stable version for ESP32C3_SUPERMINI and ESP32-S3 Welded Zero 
 Changes V008: On Github
 Changes V009: Added Selftest in menu under option H. Updated webpage.h 
 Changes V010: Remove CR+LF from ReworkInputString. BLE delay(40) -> 10
 Changes V011: Removed CR+LF from ReworkInputString with trim(). BLE --> NIMBLE v2 (not )

How to compile: Install ESP32 boards in Tools ->Board ->Boards manager
Board: ESP32C3 Dev Module or ESP32S3 Dev Module
CDC On Boot: Enabled (if you want a serial monitor connection via the USB port)
Partition Scheme: Minimal SPIFFS (1.9 MB app)
                  OTA is required 
-------------------------------------------------------------------------------------
To use the USB CDC, you need to configure your device in the Tools menu:
1) Enter into Download Mode manually
   Similar to the DFU mode, you will need to enter into download mode manually. 
   To enter into the download mode, you need to press and hold BOOT button and press and release the RESET button.
2) Configure the USB CDC
  For ESP32-S2    USB CDC On Boot -> Enabled
                  Upload Mode -> Internal USB
  For ESP32-C3    USB CDC On Boot -> Enabled
  For ESP32-S3    USB CDC On Boot -> Enabled
                  Upload Mode -> UART0 / Hardware CDC
3)Flash and Monitor
  You can now upload your sketch to the device. After flashing for the first time, you need to manually reset the device.
  This procedure enables the flashing and monitoring through the internal USB and does not requires you to manually enter into the download mode or to do the manual reset after flashing.
  To monitor the device, you need to select the USB port and open the Monitor tool selecting the correct baud rate (usually 115200) according to the Serial.begin() defined in your code.

4) Perform a reset with option R in the menu of the program after the first upload to be sure everything is set to their default settings


-------------------------------------------------------------------------------------

ToDo:
Help scherm uitprinten
Store serial-print per dag op SD per maand for debugging purposes
 
Usage of:
1 
2 BLE nRF UART connection with phone
4 RGB LED
6 Preferences or SPIFFS storage of a struct with settings
7 Get timezone corrected time with daylight savings from a NTP server via WIFI
8 Menu driven with serial monitor, BLE and WIFI
9 Change connection settings of WIFI via BLE
10 Get time with NTP server via WIFI
11 OTA updates of the software by WIFI

*/
// =============================================================================================================================

//#define ESP32C3_SUPERMINI
#define ESP32S3_ZERO

// ------------------>   Define only one type of LED strip 
//#define LED2812
#define LED6812                                    // Choose between LED type RGB=LED2812 or RGBW=LED6812
// ------------------>   Define only one library

// ------------------>   Define How many LEDs in fibonacci clock
const int NUM_LEDS = 14;                          // How many leds in fibonacci clock?
    // If Not 14 LEDs --->  Update also LED positions in  setPixel() (at the end of file) TOO !!  

//------------------------------------------------------------------------------
// PIN Assignments for Arduino Nano ESP32
//------------------------------------------------------------------------------ 

                                   #ifdef ESP32C3_SUPERMINI

#define  PhotoCellPin 20                                            // LDR Photocell pin
#define  LED_PIN      4                                             // GPIO 1 Pin to control colour SK6812/WS2812 LEDs
#define  secondsPin   3                                             // If LED is connected to Pin IO3 with 470 ohm resistor
                                   #endif // ESP32C3_SUPERMINI

                                   #ifdef ESP32S3_ZERO
#define  PhotoCellPin 2                                             // LDR Photocell pin
#define  LED_PIN      1                                             // GPIO 1 Pin to control colour SK6812/WS2812 LEDs
#define  secondsPin   3                                             // If LED is connected to Pin IO1 with 470 ohm resistor
                                   #endif // ESP32S3_ZERO



//--------------------------------------------
// ESP32 Includes defines and initialisations
//--------------------------------------------

#include <Preferences.h>
#include <Adafruit_NeoPixel.h> // https://github.com/adafruit/Adafruit_NeoPixel   for LED strip WS2812 or SK6812
#include <NimBLEDevice.h>      // For BLE communication  https://github.com/h2zero/NimBLE-Arduino
#include <ESPNtpClient.h>      // https://github.com/gmag11/ESPNtpClient
#include <WiFi.h>              // Used for web page 
#include "esp_wps.h"           // WPS functionality
#include <AsyncTCP.h>          // Used for webpage   https://github.com/me-no-dev/ESPAsyncWebServer
#include <ESPAsyncWebServer.h> // Used for webpage   https://github.com/me-no-dev/ESPAsyncWebServer
#include <ElegantOTA.h>        // If compile error see here :https://docs.elegantota.pro/async-mode/  Used for OTA new version // #include <AsyncElegantOTA.h>   // Used for OTA old version


//--------------------------------------------
// ESP32 Save Data Permanently using Preferences
//--------------------------------------------
Preferences FLASHSTOR;
 //--------------------------------------------
// COLOURS stored in Mem.DisplayChoice
//--------------------------------------------   

const byte DEFAULTCOLOUR = 0;
const byte HOURLYCOLOUR  = 1;          
const byte WHITECOLOR    = 2;
const byte OWNCOLOUR     = 3;
const byte OWNHETISCLR   = 4;
const byte WHEELCOLOR    = 5;
const byte DIGITAL       = 6;
const byte ANALOOG       = 7;



#define CLOCK_PIXELS    5                                           // Number of cells in clock = 5 (1,1,2,3,5)
#define DISPLAY_PALETTE 1                                           // Default palette to start with
bool    FiboChrono =  true;                                         // true = Fibonacci, false = chrono clock display
byte    NormalExtremeUltimate = 0;                                  // 0 = Normal, 1 = Extreme, 2 = Ultimate display of colours                
byte    bits[CLOCK_PIXELS+1];                                       // Stores the hours=1 and minutes = 2 to set in LEDsetTime(byte hours, byte minutes)
byte    BitSet[CLOCK_PIXELS+1];                                     // For calculation of the bits to set

                         #ifdef LED2812
const uint32_t white  = 0xFFFFFF, wgray  = 0xAAAAAA,lgray  = 0x666666;               // R, G and B on together gives white light
const uint32_t gray   = 0x333333, dgray  = 0x222222;                
                         #endif  //LED2812
                         #ifdef LED6812    
const uint32_t white  = 0xFF000000, wgray  = 0xAA000000, lgray  = 0x66000000;           // The SK6812 LED has a white LED that is pure white
const uint32_t dgray  = 0x22000000, gray   = 0x33000000;
                         #endif  //LED6812  
//------------------------------------------------------------------------------
const uint32_t black    = 0x000000, darkorange    = 0xFF8C00, red    = 0xFF0000, chartreuse = 0x7FFF00;
const uint32_t brown    = 0x503000, cyberyellow   = 0xFFD300, orange = 0xFF8000; 
const uint32_t yellow   = 0xFFFF00, cadmiumyellow = 0xFFF600, dyellow= 0xFFAA00, chromeyellow = 0xFFA700;
const uint32_t green    = 0x00FF00, brightgreen   = 0x66FF00, apple  = 0x80FF00, grass  = 0x00FF80;  
const uint32_t amaranth = 0xE52B50, edamaranth    = 0xFF0050, amber  = 0xFF7E00;
const uint32_t marine   = 0x0080FF, darkviolet    = 0x800080, pink   = 0xFF0080, purple = 0xFF00FF; 
const uint32_t blue     = 0x0000FF, cerulean      = 0x007BA7, sky    = 0x00FFFF, capri  = 0x00BFFF;
const uint32_t edviolet = 0X7500BC, frenchviolet  = 0X8806CE, coquelicot = 0xFF3800;
const uint32_t greenblue= 0x00F2A0, hotmagenta    = 0xFF00BF, dodgerblue = 0x0073FF, screamingreen= 0x70FF70;

const uint32_t colors[][5] =                                        // The colour palettes
   {//off   hours   minutes both;
   { white, red   , yellow , blue  , green },  // #0 Mondriaan
   { white, red   , dyellow, blue  , green },  // #1 Mondriaan1 
   { white, red   , green  , blue  , green },  // #2 RGB  
   { white, apple , green  , grass , blue  },  // #3 Greens
   { white, red   , grass  , purple, green },  // #4 Pastel                                                                 
   { white, orange, green  , marine, blue  },  // #5 Modern
   { white, sky   , purple , blue  , green },  // #6 Cold
   { white, red   , yellow , orange, green },  // #7 Warm
   { wgray, red   , dyellow, blue  , white },  // #8 Mondriaan 2
   { wgray, red   , yellow,  blue  , white }}; // #9 Mondriaan 3  

//------------------------------------------------------------------------------
// LED
//------------------------------------------------------------------------------

const byte BRIGHTNESS    =  32;                                                            // BRIGHTNESS 0 - 255
                            #ifdef LED6812
Adafruit_NeoPixel LEDstrip = Adafruit_NeoPixel(NUM_LEDS, LED_PIN, NEO_GRBW + NEO_KHZ800);  //NEO_RGBW
                            #endif  //LED6812
                            #ifdef LED2812
Adafruit_NeoPixel LEDstrip = Adafruit_NeoPixel(NUM_LEDS, LED_PIN, NEO_GRB + NEO_KHZ800);      // NEO_RGB NEO_GRB
                            #endif  //LED2812
                                   #ifdef ESP32S3_ZERO
Adafruit_NeoPixel PCB_LED = Adafruit_NeoPixel(1, 21, NEO_RGB + NEO_KHZ800);                   // PCB LED op GPIO21
                                   #endif // ESP32S3_ZERO
                                   
bool     LEDsAreOff            = false;                                                       // If true LEDs are off except time display
bool     NoTextInColorLeds     = false;                                                       // Flag to control printing of the text in function ColorLeds()
int      Previous_LDR_read     = 512;                                                         // The actual reading from the LDR + 4x this value /5


//------------------------------------------------------------------------------              //
// LDR PHOTOCELL
//------------------------------------------------------------------------------
const byte SLOPEBRIGHTNESS    = 80;                                                           // Steepness of with luminosity of the LED increases
const int  MAXBRIGHTNESS      = 255;                                                          // Maximun value in bits  for luminosity of the LEDs (1 - 255)
const byte LOWBRIGHTNESS      = 5;                                                            // Lower limit in bits of Brightness ( 0 - 255)   
byte       TestLDR            = 0;                                                            // If true LDR inf0 is printed every second in serial monitor
int        OutPhotocell;                                                                      // stores reading of photocell;
int        MinPhotocell       = 999;                                                          // stores minimum reading of photocell;
int        MaxPhotocell       = 1;                                                            // stores maximum reading of photocell;
uint32_t   SumLDRreadshour    = 0;
uint32_t   NoofLDRreadshour   = 0;

//--------------------------------------------
// CLOCK initialysations
//--------------------------------------------                                 

static uint32_t msTick;                                                                       // Number of millisecond ticks since we last incremented the second counter
byte      lastminute = 0, lasthour = 0, lastday = 0, sayhour = 0;
bool      Zelftest             = false;
struct    tm timeinfo;                                                                        // storage of time 

//--------------------------------------------                                                //
// BLE   //#include <NimBLEDevice.h>
//--------------------------------------------
BLEServer *pServer      = NULL;
BLECharacteristic * pTxCharacteristic;
bool deviceConnected    = false;
bool oldDeviceConnected = false;
std::string ReceivedMessageBLE;

#define SERVICE_UUID           "6E400001-B5A3-F393-E0A9-E50E24DCCA9E"                         // UART service UUID
#define CHARACTERISTIC_UUID_RX "6E400002-B5A3-F393-E0A9-E50E24DCCA9E"
#define CHARACTERISTIC_UUID_TX "6E400003-B5A3-F393-E0A9-E50E24DCCA9E"

//----------------------------------------
// WEBSERVER
//----------------------------------------
bool SSIDandNetworkfound = false;                                                             // True if Mem.SSID and a found WIFI.scan'ed network are identical
bool WIFIConnected = false;                                                                   // Is wIFI connected?
AsyncWebServer server(80);                                                                    // For OTA Over the air uploading
#include "Webpage.h"

//--------------------------------------------                                                //
// NTP
//----------------------------------------
boolean syncEventTriggered = false;                                                           // True if a time even has been triggered
NTPEvent_t ntpEvent;                                                                          // Last triggered event
//----------------------------------------
// Common
//----------------------------------------
 uint64_t Loopcounter = 0;
#define MAXTEXT 140
char sptext[MAXTEXT];                                                                        // For common print use 
bool SerialConnected = true;   
struct EEPROMstorage {                                                                       // Data storage in EEPROM to maintain them after power loss
  byte DisplayChoice    = 0;
  byte TurnOffLEDsAtHH  = 0;
  byte TurnOnLEDsAtHH   = 0;
  byte LanguageChoice   = 0;
  byte LightReducer     = 0;
  int  LowerBrightness  = 0;
  int  UpperBrightness  = 0;
  int  NVRAMmem[24];                                                                          // LDR readings
  byte BLEOn            = 1;
  byte NTPOn            = 1;
  byte WIFIOn           = 1;  
  byte StatusLEDOn      = 1;
  int  ReconnectWIFI    = 0;                                                                  // No of times WIFI reconnected 
  byte UseSDcard        = 0;
  byte WIFINoOfRestarts = 0;                                                                  // If 1 than resart MCU once
  byte FiboChrono       = 0;                                                                  // true = Fibonacci, false = chrono clock display
  byte NoExUl           = 0;                                                                  // 0 = Normal, 1 = Extreme, 2 = Ultimate display of colours
  byte ByteFuture3      = 0;                                                                  // For future use
  byte ByteFuture4      = 0;                                                                  // For future use
  int  IntFuture1       = 0;                                                                  // For future use
  int  IntFuture2       = 0;                                                                  // For future use
  int  IntFuture3       = 0;                                                                  // For future use
  int  IntFuture4       = 0;                                                                  // For future use   
  byte UseBLELongString = 0;                                                                  // Send strings longer than 20 bytes per message. Possible in IOS app BLEserial Pro 
  uint32_t OwnColour    = 0;                                                                  // Self defined colour for clock display
  uint32_t DimmedLetter = 0;
  uint32_t BackGround   = 0;
  char SSID[30];                                                                              // 
  char Password[40];                                                                          // 
  char BLEbroadcastName[30];                                                                  // Name of the BLE beacon
  char Timezone[50];
  int  Checksum        = 0;
}  Mem; 
//--------------------------------------------                                                //
// Menu
//0        1         2         3         4
//1234567890123456789012345678901234567890----  
 char menu[][40] = {
 "A SSID B Password C BLE beacon name",
 "D Date (D15012021) T Time (T132145)",
//    "E Normal, Extreme or Ultimate mode",
//    "F Fibonacci or Chrono display",
 "E Timezone  (E<-02>2 or E<+01>-1)",
 "H Self test",
 "I To print this Info menu",
 "K LDR reads/sec toggle On/Off", 
 "N Display off between Nhhhh (N2208)",
 "O Display toggle On/Off",
                                    #ifdef ESP32S3_ZERO
 "P Status LED toggle On/Off", 
                                    #endif // ESP32S3_ZERO
 "Q Display colour choice (Q for options)",
 "R Reset settings @ = Reset MCU",
 "--Light intensity settings (1-250)--",
 "S=Slope L=Min  M=Max   (S80 V5 U200)",
 "W=WIFI  X=NTP& CCC=BLE  +=Fast BLE", 
 "Ed Nieuwenhuys April 2024" };
 
//  -------------------------------------   End Definitions  ---------------------------------------

//--------------------------------------------                                                //
// ARDUINO Setup
//--------------------------------------------
void setup() 
{
 Serial.begin(115200);                                                                        // Setup the serial port to 115200 baud //
 int32_t Tick = millis();  
 while (!Serial)  {if ((millis() - Tick) >2000) break;}                                       // Wait until serial port is started   
 Tekstprintln("Serial started");         
 SetStatusLED(255,0,0);                                                                         // Set the status LED to red
 StartLeds();                                                                                 // LED RainbowCycle
 pinMode(secondsPin, OUTPUT);                                                                 // turn On seconds LED-pin
 Previous_LDR_read = ReadLDR();
 InitStorage();                           Tekstprintln("Setting loaded");                     // Load settings from storage and check validity 
 if(Mem.BLEOn) StartBLEService();      Tekstprintln("BLE started");                           // Start BLE service
 if(Mem.WIFIOn){ Tekstprintln("Starting WIFI");  StartWIFI_NTP();   }                         // Start WIFI and optional NTP if Mem.WIFIOn = 1 
 SWversion();                                                                                 // Print the menu + version 
 GetTijd(0);                                                                                  // 
 Print_RTC_tijd();
 Displaytime();                                                                               // Print the tekst time in the display 
 msTick = millis(); 
}
//--------------------------------------------                                                //
// ARDUINO Loop
//--------------------------------------------
void loop() 
{
 Loopcounter++;
 EverySecondCheck();  
 CheckDevices();
}
//--------------------------------------------                                                //
// COMMON Check connected input devices
//--------------------------------------------
void CheckDevices(void)
{
 CheckBLE();                                                                                  // Something with BLE to do?
 SerialCheck();                                                                               // Check serial port every second 
 ElegantOTA.loop();                                                                           // For Over The Air updates This loop block is necessary for ElegantOTA to handle reboot after OTA update.
// ButtonsCheck();                                                                            // Check if buttons pressed
}
//--------------------------------------------                                                //
// COMMON Update routine done every second
//--------------------------------------------
void EverySecondCheck(void)
{
 static bool Toggle = 1;
 uint32_t msLeap = millis() - msTick;                                                         // 
 if (msLeap >999)                                                                             // Every second enter the loop
 {
  GetTijd(0); 
  msTick = millis();
  Toggle = !Toggle;
  digitalWrite(secondsPin, Toggle);                                                          // turn the LED off by making the voltage LOW
  if (Mem.StatusLEDOn)  SetStatusLED(0,WIFIConnected*255,deviceConnected*255);  
  DimLeds(TestLDR);                                                                          // Every second an intensitiy check and update from LDR reading 
  if (timeinfo.tm_min != lastminute) EveryMinuteUpdate();                                    // Enter the every minute routine after one minute; 
//if (Loopcounter < 10) ESP.restart();
  Loopcounter=0;
 }  
}
//--------------------------------------------                                                //
// COMMON Update routine done every minute
//-------------------------------------------- 
void EveryMinuteUpdate(void)
{   
 lastminute = timeinfo.tm_min;  
  if(Mem.WIFIOn)                                                                              // If WIFI switch is On.
   {
    if(WiFi.localIP()[0] == 0)
       {
        if(WIFIConnected)  WiFi.reconnect();                                                  // If connection lost and WIFI is used reconnect
        if(CheckforWIFINetwork(false) && !WIFIConnected) StartWIFI_NTP();                     // If there was no WIFI at start up  
        if(WiFi.localIP()[0] != 0) 
          {
          sprintf(sptext, "Reconnected to IP address: %d.%d.%d.%d", WiFi.localIP()[0], WiFi.localIP()[1], WiFi.localIP()[2], WiFi.localIP()[3] );
          Tekstprintln(sptext);
          }
        }
   }
 GetTijd(0);
 Displaytime();
 Print_RTC_tijd();
 DimLeds(true);   
// Print_tijd();                                                                              // sprintf(sptext,"NTP:%s", NTP.getTimeDateString());   Tekstprintln(sptext);  
 //  if(WiFi.localIP()[0]>0) WIFIConnected = true; else WIFIConnected = false;                // To be sure connection is still there
 if(timeinfo.tm_hour != lasthour) EveryHourUpdate();
}
//------------------------------------------------------------------------------              //
// COMMON Update routine done every hour
//--------------------------------------------
void EveryHourUpdate(void)
{
 lasthour = timeinfo.tm_hour;
 if (!Mem.StatusLEDOn) SetStatusLED(0,0,0);                                                   // If for some reason the LEDs are ON and after a MCU restart turn them off.  
 if(timeinfo.tm_hour == Mem.TurnOffLEDsAtHH){ LEDsAreOff = true;  ClearScreen(); }            // Is it time to turn off the LEDs?
 if(timeinfo.tm_hour == Mem.TurnOnLEDsAtHH)   LEDsAreOff = false;                             // 
 Mem.NVRAMmem[lasthour] =(byte)((SumLDRreadshour / NoofLDRreadshour?NoofLDRreadshour:1));     // Update the average LDR readings per hour
 SumLDRreadshour  = 0;
 NoofLDRreadshour = 0;
 if (timeinfo.tm_mday != lastday) EveryDayUpdate();  
}
//--------------------------------------------                                                //
// COMMON Update routine done every day
//--------------------------------------------
void EveryDayUpdate(void)
{
 if(timeinfo.tm_mday != lastday) 
   {
    lastday           = timeinfo.tm_mday; 
    Previous_LDR_read = ReadLDR();                                                            // to have a start value
    MinPhotocell      = Previous_LDR_read;                                                    // Stores minimum reading of photocell;
    MaxPhotocell      = Previous_LDR_read;                                                    // Stores maximum reading of photocell;
//    StoreStructInFlashMemory();                                                             // 
    }
}

//--------------------------------------------                                                //
// COMMON check for serial input
//--------------------------------------------
void SerialCheck(void)
{
 String SerialString; 
 while (Serial.available())
    { 
     char c = Serial.read();                                                                  // Serial.write(c);
     if (c>31 && c<128) SerialString += c;                                                    // Allow input from Space - Del
     else c = 0;                                                                              // Delete a CR
    }
 if (SerialString.length()>0) 
    {
     ReworkInputString(SerialString+"\n");                                                    // Rework ReworkInputString();
     SerialString = "";
    }
}

//--------------------------------------------                                                //
// COMMON Reset to default settings
//--------------------------------------------
void Reset(void)
{
 Mem.Checksum         = 25065;                                                                //
 Mem.DisplayChoice    = DEFAULTCOLOUR;                                                        // Default colour scheme 
 Mem.OwnColour        = green;                                                                // Own designed colour.
 Mem.DimmedLetter     = dgray;
 Mem.BackGround       = black; 
 Mem.LanguageChoice   = 4;                                                                    // 0 = NL, 1 = UK, 2 = DE, 3 = FR, 4 = Wheel
 Mem.LightReducer     = SLOPEBRIGHTNESS;                                                      // Factor to dim ledintensity with. Between 0.1 and 1 in steps of 0.05
 Mem.UpperBrightness  = MAXBRIGHTNESS;                                                        // Upper limit of Brightness in bits ( 1 - 1023)
 Mem.LowerBrightness  = LOWBRIGHTNESS;                                                        // Lower limit of Brightness in bits ( 0 - 255)
 Mem.TurnOffLEDsAtHH  = 0;                                                                    // Display Off at nn hour
 Mem.TurnOnLEDsAtHH   = 0;                                                                    // Display On at nn hour Not Used
 Mem.FiboChrono       = true;                                                                 // true = Fibonacci, false = chrono clock display
 Mem.NoExUl           = 0;                                                                    // 0 = Normal, 1 = Extreme, 2 = Ultimate display of colours
 Mem.BLEOn            = 1;                                                                    // BLE On
 Mem.UseBLELongString = 0;
 Mem.NTPOn            = 0;                                                                    // NTP On
 Mem.WIFIOn           = 0;                                                                    // WIFI On  
 Mem.ReconnectWIFI    = 0;                                                                    // No of times WIFI reconnected
 Mem.WIFINoOfRestarts = 0;                                                                    //  
 Mem.UseSDcard        = 0;
 Previous_LDR_read    = ReadLDR();                                                            // Read LDR to have a start value. max = 4096/8 = 255
 MinPhotocell         = Previous_LDR_read;                                                    // Stores minimum reading of photocell;
 MaxPhotocell         = Previous_LDR_read;                                                    // Stores maximum reading of photocell;                                            
 TestLDR              = 0;                                                                    // If true LDR display is printed every second
 
 strcpy(Mem.SSID,"Guest");                                                                         // Default SSID
 strcpy(Mem.Password,"fibonacci");                                                                     // Default password
 strcpy(Mem.BLEbroadcastName,"FiboESP32");
 strcpy(Mem.Timezone,"CET-1CEST,M3.5.0,M10.5.0/3");                                           // Central Europe, Amsterdam, Berlin etc.                                                         // WIFI On  

 Tekstprintln("**** Reset of preferences ****"); 
 StoreStructInFlashMemory();                                                                  // Update Mem struct       
 GetTijd(0);                                                                                  // Get the time and store it in the proper variables
 SWversion();                                                                                 // Display the version number of the software
 Displaytime();
}
//--------------------------------------------                                                //
// COMMON common print routines
//--------------------------------------------
void Tekstprint(char const *tekst)    { if(Serial) Serial.print(tekst);  SendMessageBLE(tekst);sptext[0]=0;   } 
void Tekstprintln(char const *tekst)  { sprintf(sptext,"%s\n",tekst); Tekstprint(sptext);  }
void TekstSprint(char const *tekst)   { printf(tekst); sptext[0]=0;}                          // printing for Debugging purposes in serial monitor 
void TekstSprintln(char const *tekst) { sprintf(sptext,"%s\n",tekst); TekstSprint(sptext); }
//--------------------------------------------                                                //
//  COMMON String upper
//--------------------------------------------
void to_upper(char* string)
{
 const char OFFSET = 'a' - 'A';
 while (*string) {(*string >= 'a' && *string <= 'z') ? *string -= OFFSET : *string;   string++;  }
}
//--------------------------------------------                                                //
// COMMON Constrain a string with integers
// The value between the first and last character in a string is returned between the low and up bounderies
//--------------------------------------------
int SConstrainInt(String s,byte first,byte last,int low,int up){return constrain(s.substring(first, last).toInt(), low, up);}
int SConstrainInt(String s,byte first,          int low,int up){return constrain(s.substring(first).toInt(), low, up);}
//--------------------------------------------                                                //
// COMMON Init and check contents of EEPROM
//--------------------------------------------
void InitStorage(void)
{
 // if(!SPIFFS.begin(FORMAT_SPIFFS_IF_FAILED)){ Tekstprintln("Card Mount Failed");   return;}
 // else Tekstprintln("SPIFFS mounted"); 

 GetStructFromFlashMemory();
 if( Mem.Checksum != 25065)
   {
    sprintf(sptext,"Checksum (25065) invalid: %d\n Resetting to default values",Mem.Checksum); 
    Tekstprintln(sptext); 
    Reset();                                                                                  // If the checksum is NOK the Settings were not set
   }
 Mem.LanguageChoice  = _min(Mem.LanguageChoice, 4);                                           // Constrain the value to valid ranges 
 Mem.DisplayChoice   = _min(Mem.DisplayChoice, ANALOOG);                                      // Constrain the value to valid ranges 
 if(Mem.OwnColour == 0) Mem.OwnColour = green;                                                // If memory is empty cq black colour then store default value, green  
 Mem.LightReducer    = constrain(Mem.LightReducer,1,250);                                     // 
 Mem.LowerBrightness = constrain(Mem.LowerBrightness, 1, 250);                                // 
 Mem.UpperBrightness = _min(Mem.UpperBrightness, 255); 
 if(strlen(Mem.Password)<5 || strlen(Mem.SSID)<3)     Mem.WIFIOn = Mem.NTPOn = 0;       // If ssid or password invalid turn WIFI/NTP off
 
 StoreStructInFlashMemory();
}
//--------------------------------------------                                                //
// COMMON Store mem.struct in FlashStorage or SD
// Preferences.h  
//--------------------------------------------
void StoreStructInFlashMemory(void)
{
  FLASHSTOR.begin("Mem",false);       //  delay(100);
  FLASHSTOR.putBytes("Mem", &Mem , sizeof(Mem) );
  FLASHSTOR.end();          
  
// Can be used as alternative
//  SPIFFS
//  File myFile = SPIFFS.open("/MemStore.txt", FILE_WRITE);
//  myFile.write((byte *)&Mem, sizeof(Mem));
//  myFile.close();
 }
//--------------------------------------------                                                //
// COMMON Get data from FlashStorage
// Preferences.h
//--------------------------------------------
void GetStructFromFlashMemory(void)
{
 FLASHSTOR.begin("Mem", false);
 FLASHSTOR.getBytes("Mem", &Mem, sizeof(Mem) );
 FLASHSTOR.end(); 

// Can be used as alternative if no SD card
//  File myFile = SPIFFS.open("/MemStore.txt");  FILE_WRITE); myFile.read((byte *)&Mem, sizeof(Mem));  myFile.close();

 sprintf(sptext,"Mem.Checksum = %d",Mem.Checksum);Tekstprintln(sptext); 
}
//--------------------------------------------                                                //
//  COMMON Input from Bluetooth or Serial
//--------------------------------------------
void ReworkInputString(String InputString)
{
 if(InputString.length()> 40){Serial.printf("Input string too long (max40)\n"); return;}      // If garbage return
 InputString.trim();                                                                          // Remove CR, LF etc.
 sptext[0] = 0;                                                                               // Empty the sptext string
                                                                                              // Empty the sptext string
 if(InputString[0] > 31 && InputString[0] <127)                                               // Does the string start with a letter?
  { 
  switch (InputString[0])
   {
    case 'A':
    case 'a': 
            if (InputString.length() >4 )
            {
             InputString.substring(1).toCharArray(Mem.SSID,InputString.length());
             sprintf(sptext,"SSID set: %s", Mem.SSID);  
            }
            else sprintf(sptext,"**** Length fault. Use between 4 and 30 characters ****");
            break;
    case 'B':
    case 'b': 
           if (InputString.length() >4 )
            {  
             InputString.substring(1).toCharArray(Mem.Password,InputString.length());
             sprintf(sptext,"Password set: %s\n Enter @ to reset ESP32 and connect to WIFI and NTP", Mem.Password); 
             Mem.NTPOn        = 1;                                                         // NTP On
             Mem.WIFIOn       = 1;                                                         // WIFI On  
            }
            else sprintf(sptext,"%s,**** Length fault. Use between 4 and 40 characters ****",Mem.Password);
            break;   
    case 'C':
    case 'c': 
             if (InputString.equalsIgnoreCase("ccc"))                                         // Toggle BLE ON or OFF
               {   
                Mem.BLEOn = 1 - Mem.BLEOn; 
                sprintf(sptext,"BLE is %s after restart", Mem.BLEOn?"ON":"OFF" );
               }    
             if (InputString.length() >4 )
               {  
                InputString.substring(1).toCharArray(Mem.BLEbroadcastName,InputString.length());
                sprintf(sptext,"BLE broadcast name set: %s", Mem.BLEbroadcastName); 
                Mem.BLEOn = 1;                                                                // BLE On
              }
            else sprintf(sptext,"**** Length fault. Use between 4 and 30 characters ****");
            break;       
    case 'D':
    case 'd':  
            if (InputString.length() == 9 )
              {
               timeinfo.tm_mday = (int) SConstrainInt(InputString,1,3,0,31);
               timeinfo.tm_mon  = (int) SConstrainInt(InputString,3,5,0,12) - 1; 
               timeinfo.tm_year = (int) SConstrainInt(InputString,5,9,2000,9999) - 1900;
               SetRTCTime();
               Print_tijd();  //Tekstprintln(sptext);                                         // Print_Tijd() fills sptext with time string
              }
            else sprintf(sptext,"****\nLength fault. Enter Dddmmyyyy\n****");
            break;
//--------------------------------------------                                                //
/*       
   case 'E':
   case 'e':
            if(InputString.length() == 1)
              {
               if (++Mem.NoExUl>2) Mem.NoExUl=0;
               sprintf(sptext,"Only working in chrono modus\nDisplay is %s", Mem.NoExUl==1?"Extreme":Mem.NoExUl==2?"Ultimate":"Normal" );
              }
            break;    
   case 'F':
   case 'f':
            if(InputString.length() == 1)
              {
               Mem.FiboChrono = !Mem.FiboChrono;
               sprintf(sptext,"Display is %s", Mem.FiboChrono?"Fibonacci":"Chrono" );
              }
            break;
 */

    case 'G':
    case 'g':  
 
             break;
    case 'H':
    case 'h': Play_Lights(); 
              break;            
    case 'I':
    case 'i': 
            SWversion();
            break;
    case 'K':
    case 'k':
             TestLDR = 1 - TestLDR;                                                           // If TestLDR = 1 LDR reading is printed every second instead every 30s
             sprintf(sptext,"TestLDR: %s \nLDR reading, min and max of one day, %%Out, loops per second and time",TestLDR? "On" : "Off");
             break;      
    case 'L':                                                                                 // Language to choose
    case 'l':
             if (InputString.length() < 5)
               {      
                Mem.LowerBrightness = (byte) SConstrainInt(InputString,1,0,255);
                sprintf(sptext,"Lower brightness: %d bits",Mem.LowerBrightness);
               }
             break;       
    case 'M':
    case 'm':
            if (InputString.length() < 5)
               {    
                Mem.UpperBrightness = SConstrainInt(InputString,1,1,255);
                sprintf(sptext,"Upper brightness changed to: %d bits",Mem.UpperBrightness);
               }
              break;  
    case 'N':
    case 'n':
             if (InputString.length() == 1 )         Mem.TurnOffLEDsAtHH = Mem.TurnOnLEDsAtHH = 0;
             if (InputString.length() == 5 )
              {
               Mem.TurnOffLEDsAtHH =(byte) InputString.substring(1,3).toInt(); 
               Mem.TurnOnLEDsAtHH  =(byte) InputString.substring(3,5).toInt(); 
              }
             Mem.TurnOffLEDsAtHH = _min(Mem.TurnOffLEDsAtHH, 23);
             Mem.TurnOnLEDsAtHH  = _min(Mem.TurnOnLEDsAtHH, 23); 
             sprintf(sptext,"Display is OFF between %2d:00 and %2d:00", Mem.TurnOffLEDsAtHH,Mem.TurnOnLEDsAtHH );
 //            Tekstprintln(sptext); 
             break;
    case 'O':
    case 'o':
             if(InputString.length() == 1)
               {
                LEDsAreOff = !LEDsAreOff;
                sprintf(sptext,"Display is %s", LEDsAreOff?"OFF":"ON" );
                if(LEDsAreOff) { ClearScreen();}                                              // Turn the display off
                else {
                  Tekstprintln(sptext); 
                  Displaytime();                                                              // Turn the display on                
                }
               }
             break;                                                                   
    case 'p':
    case 'P':  
             if(InputString.length() == 1)
               {
                Mem.StatusLEDOn = !Mem.StatusLEDOn;
                SetStatusLED(0,0,0); 
                sprintf(sptext,"StatusLEDs are %s", Mem.StatusLEDOn?"ON":"OFF" );               
               }
             break; 
    case 'q':
    case 'Q':   
             if (InputString.length() == 1 )
               {
             Tekstprintln("  Q0= Mondriaan1");
             Tekstprintln("  Q1= Mondriaan2");
             Tekstprintln("  Q2= RGB");
             Tekstprintln("  Q3= Greens");
             Tekstprintln("  Q4= Pastel");
             Tekstprintln("  Q5= Modern");
             Tekstprintln("  Q6= Cold");
             Tekstprintln("  Q7= Warm");
             Tekstprintln("  Q8= Mondriaan3");
             Tekstprint(  "  Q9= Mondriaan4");
             sptext[0]=0;
               }
             if (InputString.length() == 2 )
               {
                Mem.DisplayChoice = InputString.substring(1).toInt();
                sprintf(sptext,"Palette: %d",Mem.DisplayChoice);

                lastminute = 99;                                                              // Force a minute update
               }    
             break;
    case 'R':
    case 'r':
             if (InputString.length() == 1)
               {   
                Reset();
                sprintf(sptext,"\nReset to default values: Done");
                Displaytime();                                                                // Turn on the display with proper time
               }                                
             else sprintf(sptext,"**** Length fault. Enter R ****");
             break;      
    case 'S':                                                                                 // factor ( 0 - 1) to multiply brighness (0 - 255) with 
    case 's':
            if (InputString.length() < 5)
               {    
                Mem.LightReducer = (byte) SConstrainInt(InputString,1,1,255);
                sprintf(sptext,"Slope brightness changed to: %d%%",Mem.LightReducer);
               }
              break;                    
    case 'T':
    case 't':
//                                                                                            //
             if(InputString.length() == 7)  // T125500
               {
                timeinfo.tm_hour = (int) SConstrainInt(InputString,1,3,0,23);
                timeinfo.tm_min  = (int) SConstrainInt(InputString,3,5,0,59); 
                timeinfo.tm_sec  = (int) SConstrainInt(InputString,5,7,0,59);
                SetRTCTime();
                Print_tijd(); 
               }
             else sprintf(sptext,"**** Length fault. Enter Thhmmss ****");
             break;            
    // case 'U':                                                                              // factor to multiply brighness (0 - 255) with 
    // case 'u':

    // case 'V':
    // case 'v':  
 
    case 'W':
    case 'w':
             if (InputString.length() == 1)
               {   
                Mem.WIFIOn = 1 - Mem.WIFIOn; 
                Mem.ReconnectWIFI = 0;                                                       // Reset WIFI reconnection counter 
                sprintf(sptext,"WIFI is %s after restart", Mem.WIFIOn?"ON":"OFF" );
               }                                
             else sprintf(sptext,"**** Length fault. Enter W ****");
             break; 
    case 'X':
    case 'x':
             if (InputString.length() == 1)
               {   
                Mem.NTPOn = 1 - Mem.NTPOn; 
                sprintf(sptext,"NTP is %s after restart", Mem.NTPOn?"ON":"OFF" );
               }                                
             else sprintf(sptext,"**** Length fault. Enter X ****");
             break; 
    // case 'Y':
    // case 'y':                        
    //          sprintf(sptext,"**** Input fault. Enter Y ****");
    //          break; 
    case '+':
             if (InputString.length() == 1)
               {   
                Mem.UseBLELongString = 1 - Mem.UseBLELongString; 
                sprintf(sptext,"Fast BLE is %s", Mem.UseBLELongString?"ON":"OFF" );
               }                                
             else sprintf(sptext,"**** Length fault. Enter U ****");
             break; 
//--------------------------------------------                                                //        
    case '@':
             if (InputString.length() == 1)
               {   
               Tekstprintln("\n*********\n ESP restarting\n*********\n");
                ESP.restart();   
               }                                
             else sprintf(sptext,"**** Length fault. Enter @ ****");
             break;     
    case '#':
             if (InputString.length() == 1)
               {   
                Zelftest = 1 - Zelftest; 
                sprintf(sptext,"Zelftest: %d",Zelftest);
                Displaytime();                                                                // Turn on the display with proper time
               }                                
             else sprintf(sptext,"**** Length fault. Enter S ****");
             break; 
    case '$':
             if (InputString.length() == 1)
               {   
                Mem.UseSDcard = 1 - Mem.UseSDcard; 
                sprintf(sptext,"SD is %s after restart", Mem.UseSDcard?"used":"NOT used" );
                sprintf(sptext,"This function is not working, .... yet" );
               }                                
             else sprintf(sptext,"**** Length fault. Enter Z ****");
             break; 
    case '&':                                                                                // Get NTP time
             if (InputString.length() == 1)
              {
               NTP.getTime();                                                                // Force a NTP time update      
               Tekstprint("NTP query started. \nClock time: ");
               delay(500);          
               GetTijd(1);
               Tekstprint("\n  NTP time: ");
               PrintNTP_tijd();
               Tekstprintln("");
               } 
             break;
    case '0':
    case '1':
    case '2':        
             if (InputString.length() == 6)                                                  // For compatibility input with only the time digits
              {
               timeinfo.tm_hour = (int) SConstrainInt(InputString,0,2,0,23);
               timeinfo.tm_min  = (int) SConstrainInt(InputString,2,4,0,59); 
               timeinfo.tm_sec  = (int) SConstrainInt(InputString,4,6,0,59);
               sprintf(sptext,"Time set");  
               SetRTCTime();
               Print_RTC_tijd(); 
               } 
    default: break;
    }
  }  
 Tekstprintln(sptext); 
 StoreStructInFlashMemory();                                                                  // Update EEPROM                                     
 InputString = "";
}
//--------------------------------------------                                                //
// LDR reading are between 0 and 255. 
// ESP32 analogue read is between 0 - 4096 --   is: 4096 / 8
//--------------------------------------------
int ReadLDR(void)
{
 return analogRead(PhotoCellPin)/16;
}
//--------------------------------------------
// LED CLOCK Control the LEDs on the Nano ESP32
// 0 Low is LED off. Therefore the value is inversed with the ! Not 
//--------------------------------------------
void SetStatusLED(uint32_t Red, uint32_t Green, uint32_t Blue)
{
                                     #ifdef ESP32S3_ZERO
  PCB_LED.fill(FuncCRGBW(Red,Green,Blue,0), 0, 1);
  PCB_LED.setBrightness(32);                                                                  // 255 = 100%
  PCB_LED.show();
                                     #endif //ESP32S3_ZERO
}
// --------------------Colour Clock Light functions -----------------------------------
//--------------------------------------------                                                //
//  LED Set color for LEDs in strip and print tekst
//---------------------------------------------
void ColorLeds(char const *Tekst, int FirstLed, int LastLed, uint32_t RGBWColor)
{ 
 Stripfill(RGBWColor, FirstLed, ++LastLed - FirstLed );                                        //
 if (!NoTextInColorLeds && strlen(Tekst) > 0 )
     {sprintf(sptext,"%s ",Tekst); Tekstprint(sptext); }                                      // Print the text  
}
//--------------------------------------------
//  LED Set color for one LED
//--------------------------------------------
void ColorLed(int Lednr, uint32_t RGBWColor)
{   
 Stripfill(RGBWColor, Lednr, 1 );
}
//--------------------------------------------                                                //
//  LED Clear display settings of the LED's
//--------------------------------------------
void LedsOff(void) 
{ 
 Stripfill(0, 0, NUM_LEDS );                                                                  // 
}
//--------------------------------------------                                                //
// LED Turn On and Off the LED's after 200 milliseconds
//--------------------------------------------
void Laatzien()
{ 
 ShowLeds();
 delay(200);
 LedsOff(); 
 CheckDevices();                                                                              // Check for input from input devices
}

//--------------------------------------------                                                //
//  LED Push data in LED strip to commit the changes
//--------------------------------------------
void ShowLeds(void)
{
 LEDstrip.show();
}
//--------------------------------------------                                                //
//  LED Set brighness of LEDs
//--------------------------------------------
void SetBrightnessLeds(byte Bright)
{
 LEDstrip.setBrightness(Bright);                                                              // Set brightness of LEDs   
 ShowLeds();
}
//--------------------------------------------
//  LED Fill the strip array for LEDFAB library
//--------------------------------------------
void Stripfill(uint32_t RGBWColor, int FirstLed, int NoofLEDs)
{   
 LEDstrip.fill(RGBWColor, FirstLed, NoofLEDs);
}
//--------------------------------------------
//  LED Strip Get Pixel Color 
//--------------------------------------------
uint32_t StripGetPixelColor(int Lednr)
{
return(LEDstrip.getPixelColor(Lednr));
}
//--------------------------------------------                                                //
//  LED convert HSV to RGB  h is from 0-360, s,v values are 0-1
//  r,g,b values are 0-255
//--------------------------------------------
uint32_t HSVToRGB(double H, double S, double V) 
{
  int i;
  double r, g, b, f, p, q, t;
  if (S == 0)  {r = V;  g = V;  b = V; }
  else
  {
    H >= 360 ? H = 0 : H /= 60;
    i = (int) H;
    f = H - i;
    p = V * (1.0 -  S);
    q = V * (1.0 - (S * f));
    t = V * (1.0 - (S * (1.0 - f)));
    switch (i) 
    {
     case 0:  r = V;  g = t;  b = p;  break;
     case 1:  r = q;  g = V;  b = p;  break;
     case 2:  r = p;  g = V;  b = t;  break;
     case 3:  r = p;  g = q;  b = V;  break;
     case 4:  r = t;  g = p;  b = V;  break;
     default: r = V;  g = p;  b = q;  break;
    }
  }
return FuncCRGBW((int)(r*255), (int)(g*255), (int)(b*255), 0 );                                // R, G, B, W 
}
//--------------------------------------------                                                //
//  LED function to make RGBW color
//--------------------------------------------
uint32_t FuncCRGBW( uint32_t Red, uint32_t Green, uint32_t Blue, uint32_t White)
{ 
 return ( (White<<24) + (Red << 16) + (Green << 8) + Blue );
}
//--------------------------------------------                                                //
//  LED functions to extract RGBW colors
//--------------------------------------------
 uint8_t Cwhite(uint32_t c) { return (c >> 24);}
 uint8_t Cred(  uint32_t c) { return (c >> 16);}
 uint8_t Cgreen(uint32_t c) { return (c >> 8); }
 uint8_t Cblue( uint32_t c) { return (c);      }
 
//--------------------------------------------                                                //
// CLOCK 
//--------------------------------------------
void Displaytime(void)
{ 
  if (Mem.FiboChrono) LEDsetTime(timeinfo.tm_hour , timeinfo.tm_min);                         // Fibonacci display   
  else            MakeChronoList(timeinfo.tm_hour , timeinfo.tm_min, timeinfo.tm_sec);        // Chrono (clock display) display
}

//--------------------------------------------                                                //
//  LED Dim the leds measured by the LDR and print values
// LDR reading are between 0 and 255. The Brightness send to the LEDs is between 0 and 255
//--------------------------------------------
void DimLeds(bool print) 
{ 
  int LDRread = ReadLDR();                                                                    // ESP32 analoge read is between 0 - 4096, reduce it to 0-1024                                                                                                   
  int LDRavgread = (4 * Previous_LDR_read + LDRread ) / 5;                                    // Read lightsensor and avoid rapid light intensity changes
  Previous_LDR_read = LDRavgread;                                                             // by using the previous reads
  OutPhotocell = (uint32_t)((Mem.LightReducer * sqrt(255*LDRavgread))/100);                   // Linear --> hyperbolic with sqrt. Result is between 0-255
  MinPhotocell = min(MinPhotocell, LDRavgread);                                               // Lowest LDR measurement
  MaxPhotocell = max(MaxPhotocell, LDRavgread);                                               // Highest LDR measurement
  OutPhotocell = constrain(OutPhotocell, Mem.LowerBrightness, Mem.UpperBrightness);           // Keep result between lower and upper boundery en calc percentage
  SumLDRreadshour += LDRavgread;    NoofLDRreadshour++;                                       // For statistics LDR readings per hour
  if(print)
  {
//   sprintf(sptext,"LDR:%3d Avg:%3d (%3d-%3d) Out:%3d=%2d%% Loop(%ld) ",
//        LDRread,LDRavgread,MinPhotocell,MaxPhotocell,OutPhotocell,(int)(OutPhotocell/2.55),Loopcounter);    
   sprintf(sptext,"LDR:%3d (%3d-%3d) %2d%% %5lld l/s ",
        LDRread,MinPhotocell,MaxPhotocell,(int)(OutPhotocell*100/255),Loopcounter);   
   Tekstprint(sptext);
   Print_tijd();  
  }
 if(LEDsAreOff) OutPhotocell = 0;
 SetBrightnessLeds(OutPhotocell);     // values between 0 and 255
}
//--------------------------------------------                                                //
//  LED Turn On en Off the LED's
//--------------------------------------------
void Play_Lights()
{
 for(int n=0; n<2; n++) 
 {
 for(int i=0; i<NUM_LEDS; i++) { ColorLeds("",i,i,chromeyellow); ShowLeds(); delay(100); }
 WhiteOverRainbow(100,50, 5 );  // wait, whiteSpeed, whiteLength
 LedsOff();
 }
}

//--------------------------------------------                                                //
//  DISPLAY
//  Clear the display
//--------------------------------------------
void ClearScreen(void)
{
 LedsOff();
}
//--------------------------------------------                                                //
//  LED Wheel
//  Input a value 0 to 255 to get a color value.
//  The colours are a transition r - g - b - back to r.
//--------------------------------------------
uint32_t Wheel(byte WheelPos) 
{
 WheelPos = 255 - WheelPos;
 if(WheelPos < 85)   { return FuncCRGBW( 255 - WheelPos * 3, 0, WheelPos * 3, 0);  }
 if(WheelPos < 170)  { WheelPos -= 85;  return FuncCRGBW( 0,  WheelPos * 3, 255 - WheelPos * 3, 0); }
 WheelPos -= 170;      
 return FuncCRGBW(WheelPos * 3, 255 - WheelPos * 3, 0, 0);
}

//--------------------------------------------                                                //
//  LED RainbowCycle
//--------------------------------------------
void StartLeds(void) 
{
 uint16_t i, j;
 LEDstrip.setBrightness(BRIGHTNESS);                                                          // Set initial brightness of LEDs   
 for(j=0; j<256 ; j++) 
  {                                                                                           // One cycle of all colors on wheel
   for(i=0; i< NUM_LEDS; i++)  ColorLeds("",i,i,Wheel(((i * 256 / 32) + j) & 255));
   ShowLeds();
  }
}

//--------------------------------------------                                                //
//  LED RainbowCycle
//--------------------------------------------
void RainbowCycle(uint8_t wait) 
{
 uint16_t i, j;
 for(j=0; j<256 * 5; j++) 
   {                                                                                          // 5 cycles of all colors on wheel
    for(i=0; i< NUM_LEDS; i++) ColorLeds("",i,i,Wheel(((i * 256 / NUM_LEDS) + j) & 255));
    ShowLeds();
    delay(wait);
  }
}
//--------------------------------------------                                                //
//  LED WhiteOverRainbow
//--------------------------------------------
void WhiteOverRainbow(uint32_t wait, uint8_t whiteSpeed, uint32_t whiteLength ) 
{
 if(whiteLength >= NUM_LEDS) whiteLength = NUM_LEDS - 1;
 uint32_t head = whiteLength - 1;
 uint32_t tail = 0;
 uint32_t loops = 1;
 uint32_t loopNum = 0;
 static unsigned long lastTime = 0;
 while(true)
  {
    for(uint32_t j=0; j<256; j++) 
     {
      for(uint32_t i=0; i<NUM_LEDS; i++) 
       {
        if((i >= tail && i <= head) || (tail > head && i >= tail) || (tail > head && i <= head) )
              ColorLeds("",i,i,0XFFFFFF );
        else  
              ColorLeds("",i,i,Wheel(((i * 256 / NUM_LEDS) + j) & 255));
       }
      if(millis() - lastTime > whiteSpeed) 
       {
        head++;        tail++;
        if(head == NUM_LEDS) loopNum++;
        lastTime = millis();
      }
      if(loopNum == loops) return;
      head %= NUM_LEDS;
      tail %= NUM_LEDS;
      ShowLeds();
      delay(wait);
    }
  }  // end while
}
//--------------------------------------------                                                //
//  LED In- or decrease light intensity value i.e. Slope
//--------------------------------------------
void WriteLightReducer(int amount)
{
 int value = Mem.LightReducer + amount;                                                       // Prevent byte overflow by making it an integer before adding
 Mem.LightReducer = (byte) min(value,  255);                                                         // May not be larger than 255
 sprintf(sptext,"Max brightness: %3d%%",Mem.LightReducer);
 Tekstprintln(sptext);
}

//--------------------------------------------                                                //
// CLOCK Version info
//--------------------------------------------
void SWversion(void) 
{ 
 #define FILENAAM (strrchr(__FILE__, '\\') ? strrchr(__FILE__, '\\') + 1 : __FILE__)
 PrintLine(35);
 for (uint8_t i = 0; i < sizeof(menu) / sizeof(menu[0]); Tekstprintln(menu[i++]));
 PrintLine(35);
 byte dp = Mem.DisplayChoice;
 sprintf(sptext,"Display off between: %02dh - %02dh",Mem.TurnOffLEDsAtHH, Mem.TurnOnLEDsAtHH);                   Tekstprintln(sptext);
 sprintf(sptext,"Display choice: %s",dp==0?"Yellow":dp==1?"Hourly":dp==2?"White":
                      dp==3?"All Own":dp==4?"Own":dp==5?"Wheel":dp==6?"Digital":dp==7?"Analog":"NOP");            Tekstprintln(sptext);
 sprintf(sptext,"Slope: %d     Min: %d     Max: %d ",Mem.LightReducer, Mem.LowerBrightness,Mem.UpperBrightness);  Tekstprintln(sptext);
 sprintf(sptext,"SSID: %s", Mem.SSID);                                                 Tekstprintln(sptext);
// sprintf(sptext,"Password: %s", Mem.Password);                                       Tekstprintln(sptext);
 sprintf(sptext,"BLE name: %s", Mem.BLEbroadcastName);                                 Tekstprintln(sptext);
 sprintf(sptext,"IP-address: %d.%d.%d.%d (/update)", 
        WiFi.localIP()[0], WiFi.localIP()[1], WiFi.localIP()[2], WiFi.localIP()[3] );  Tekstprintln(sptext);
 sprintf(sptext,"Timezone:%s", Mem.Timezone);                                          Tekstprintln(sptext); 
 sprintf(sptext,"%s %s %s %s", Mem.WIFIOn?"WIFI=On":"WIFI=Off", 
                               Mem.NTPOn? "NTP=On":"NTP=Off",
                               Mem.BLEOn? "BLE=On":"BLE=Off",
                               Mem.UseBLELongString? "FastBLE=On":"FastBLE=Off" );     Tekstprintln(sptext);
 sprintf(sptext,"Software: %s",FILENAAM);                                              Tekstprintln(sptext);  //VERSION); 
 Print_RTC_tijd(); Tekstprintln(""); 
 PrintLine(35);
}
//--------------------------------------------                                                //
// CLOCK PrintLine
//--------------------------------------------
void PrintLine(byte Lengte)
{
 for(int n=0; n<Lengte; n++) sptext[n]='_';
 sptext[Lengte] = 0;
 Tekstprintln(sptext);
}

//--------------------------- Time functions --------------------------
//--------------------------------------------                                                //
// RTC Get time from NTP cq RTC 
// and store it in timeinfo struct
//--------------------------------------------
void GetTijd(byte printit)
{
 if(Mem.NTPOn)  getLocalTime(&timeinfo);                                                    // if NTP is running get the loacal time
 else 
  { 
   time_t now;
   time(&now);
   localtime_r(&now, &timeinfo);
  } 
 if (printit)  Print_RTC_tijd();                                                              // otherwise time in OK struct timeonfo
}
//--------------------------------------------                                                //
// RTC prints time to serial
//--------------------------------------------
void Print_RTC_tijd(void)
{
 sprintf(sptext,"%02d/%02d/%04d %02d:%02d:%02d ", 
     timeinfo.tm_mday,timeinfo.tm_mon+1,timeinfo.tm_year+1900,
     timeinfo.tm_hour,timeinfo.tm_min,timeinfo.tm_sec);
 Tekstprint(sptext);
}
//--------------------------------------------
// NTP print the NTP time for the timezone set 
//--------------------------------------------
void PrintNTP_tijd(void)
{
 sprintf(sptext,"%s  ", NTP.getTimeDateString());  
 Tekstprint(sptext);              // 17/10/2022 16:08:15
}

//--------------------------------------------
// NTP print the NTP UTC time 
//--------------------------------------------
void PrintUTCtijd(void)
{
 time_t tmi;
 struct tm* UTCtime;
 time(&tmi);
 UTCtime = gmtime(&tmi);
 sprintf(sptext,"UTC: %02d:%02d:%02d %02d-%02d-%04d  ", 
     UTCtime->tm_hour,UTCtime->tm_min,UTCtime->tm_sec,
     UTCtime->tm_mday,UTCtime->tm_mon+1,UTCtime->tm_year+1900);
 Tekstprint(sptext);   
}

//--------------------------------------------                                                //
// RTC Fill sptext with time
//--------------------------------------------
void Print_tijd(){ Print_tijd(2);}                                                            // print with linefeed
void Print_tijd(byte format)
{
 sprintf(sptext,"%02d:%02d:%02d",timeinfo.tm_hour,timeinfo.tm_min,timeinfo.tm_sec);
 switch (format)
 {
  case 0: break;
  case 1: Tekstprint(sptext); break;
  case 2: Tekstprintln(sptext); break;  
 }
}

//--------------------------------------------                                                //
// RTC Set time from global timeinfo struct
// Check if values are within range
//--------------------------------------------
void SetRTCTime(void)
{ 
 time_t t = mktime(&timeinfo);
 sprintf(sptext, "Setting time: %s", asctime(&timeinfo));    Tekstprintln(sptext);
struct timeval now = { .tv_sec = t , .tv_usec = 0};
 settimeofday(&now, NULL);
 GetTijd(0);                                                                                // Synchronize time with RTC clock
 Displaytime();
 Print_tijd();
}

//                                                                                            //
// ------------------- End  Time functions 

//--------------------------------------------                                                //
//  CLOCK Convert Hex to uint32
//--------------------------------------------
uint32_t HexToDec(String hexString) 
{
 uint32_t decValue = 0;
 int nextInt;
 for (uint8_t i = 0; i < hexString.length(); i++) 
  {
   nextInt = int(hexString.charAt(i));
   if (nextInt >= 48 && nextInt <= 57)  nextInt = map(nextInt, 48, 57, 0, 9);
   if (nextInt >= 65 && nextInt <= 70)  nextInt = map(nextInt, 65, 70, 10, 15);
   if (nextInt >= 97 && nextInt <= 102) nextInt = map(nextInt, 97, 102, 10, 15);
   nextInt = constrain(nextInt, 0, 15);
   decValue = (decValue * 16) + nextInt;
  }
 return decValue;
}


//--------------------------------------------                                                //
// BLE 
// SendMessage by BLE Slow in packets of 20 chars
// or fast in one long string.
// Fast can be used in IOS app BLESerial Pro
//------------------------------
void SendMessageBLE(std::string Message)
{
 if(deviceConnected) 
   {
    if (Mem.UseBLELongString)                                                                 // If Fast transmission is possible
     {
      pTxCharacteristic->setValue(Message); 
      pTxCharacteristic->notify();
      delay(10);                                                                              // Bluetooth stack will go into congestion, if too many packets are sent
     } 
   else                                                                                       // Packets of max 20 bytes
     {  
      int parts = (Message.length()/20) + 1;
      for(int n=0;n<parts;n++)
        {   
         pTxCharacteristic->setValue(Message.substr(n*20, 20)); 
         pTxCharacteristic->notify();
         delay(10);                                                                           // Bluetooth stack will go into congestion, if too many packets are sent
        }
     }
   } 
}

//--------------------------------------------                                                //
// BLE Start BLE Classes NimBLE Version 2.x.x
//------------------------------
class MyServerCallbacks: public NimBLEServerCallbacks 
{
 void onConnect(NimBLEServer* pServer, NimBLEConnInfo& connInfo) override 
    {deviceConnected = true; Serial.println("Connected" ); };
 void onDisconnect(NimBLEServer* pServer, NimBLEConnInfo& connInfo, int reason) override 
    {deviceConnected = false; Serial.println("NOT Connected" );}
};
  
class MyCallbacks: public NimBLECharacteristicCallbacks 
{
 void onWrite(NimBLECharacteristic *pCharacteristic, NimBLEConnInfo& connInfo) override  
  {
   std::string rxValue = pCharacteristic->getValue();
   ReceivedMessageBLE = rxValue + "\n";
  //  if (rxValue.length() > 0) {for (int i = 0; i < rxValue.length(); i++) printf("%c",rxValue[i]); }
  //  printf("\n");
  }  
};

/* /-----------------------------
// BLE Start BLE Classes NimBLE Version 1.4.3
//------------------------------
class MyServerCallbacks: public BLEServerCallbacks 
{
 void onConnect(BLEServer* pServer) {deviceConnected = true; };
 void onDisconnect(BLEServer* pServer) {deviceConnected = false;}
};

class MyCallbacks: public BLECharacteristicCallbacks 
{
 void onWrite(BLECharacteristic *pCharacteristic) 
  {
   std::string rxValue = pCharacteristic->getValue();
   ReceivedMessageBLE = rxValue + "\n";
//   if (rxValue.length() > 0) {for (int i = 0; i < rxValue.length(); i++) printf("%c",rxValue[i]); }
//   printf("\n");
  }  
};
*/
//--------------------------------------------                                                //
// BLE Start BLE Service
//------------------------------
void StartBLEService(void)
{
 BLEDevice::init(Mem.BLEbroadcastName);                                                       // Create the BLE Device
 pServer = BLEDevice::createServer();                                                         // Create the BLE Server
 pServer->setCallbacks(new MyServerCallbacks());
 BLEService *pService = pServer->createService(SERVICE_UUID);                                 // Create the BLE Service
 pTxCharacteristic                     =                                                      // Create a BLE Characteristic 
     pService->createCharacteristic(CHARACTERISTIC_UUID_TX, NIMBLE_PROPERTY::NOTIFY);                 
 BLECharacteristic * pRxCharacteristic = 
     pService->createCharacteristic(CHARACTERISTIC_UUID_RX, NIMBLE_PROPERTY::WRITE);
 pRxCharacteristic->setCallbacks(new MyCallbacks());
 pService->start(); 
 BLEAdvertising *pAdvertising = BLEDevice::getAdvertising();
 pAdvertising->addServiceUUID(SERVICE_UUID); 
 pServer->start();                                                                            // Start the server  Nodig??
 pServer->getAdvertising()->start();                                                          // Start advertising
 TekstSprint("BLE Waiting a client connection to notify ...\n"); 
}
//                                                                                            //
//-----------------------------
// BLE  CheckBLE
//------------------------------
void CheckBLE(void)
{
 if(!deviceConnected && oldDeviceConnected)                                                   // Disconnecting
   {
    delay(300);                                                                               // Give the bluetooth stack the chance to get things ready
    pServer->startAdvertising();                                                              // Restart advertising
    TekstSprint("Start advertising\n");
    oldDeviceConnected = deviceConnected;
   }
 if(deviceConnected && !oldDeviceConnected)                                                   // Connecting
   { 
    oldDeviceConnected = deviceConnected;
    SWversion();
   }
 if(ReceivedMessageBLE.length()>0)
   {
    SendMessageBLE(ReceivedMessageBLE);
    String BLEtext = ReceivedMessageBLE.c_str();
    ReceivedMessageBLE = "";
    ReworkInputString(BLEtext); 
   }
}

//--------------------------------------------                                                //
// WIFI WIFIEvents
//--------------------------------------------
void WiFiEvent(WiFiEvent_t event)
{
  const char* txtstr;
  sprintf(sptext,"[WiFi-event] event: %d", event); 
  Tekstprintln(sptext);
  WiFiEventInfo_t info;
    switch (event) 
     {
        case ARDUINO_EVENT_WIFI_READY: 
            Tekstprintln("WiFi interface ready");
            break;
        case ARDUINO_EVENT_WIFI_SCAN_DONE:
            Tekstprintln("Completed scan for access points");
            break;
        case ARDUINO_EVENT_WIFI_STA_START:
            Tekstprintln("WiFi client started");
            break;
        case ARDUINO_EVENT_WIFI_STA_STOP:
            Tekstprintln("WiFi clients stopped");
            break;
        case ARDUINO_EVENT_WIFI_STA_CONNECTED:
            Tekstprintln("Connected to access point");
            break;
       case ARDUINO_EVENT_WIFI_STA_DISCONNECTED:
            Mem.ReconnectWIFI++;
            sprintf(sptext, "Disconnected from station, attempting reconnection for the %d time", Mem.ReconnectWIFI);
            Tekstprintln(sptext);
            sprintf(sptext,"WiFi lost connection."); // Reason: %d",info.wifi_sta_disconnected.reason); 
            Tekstprintln(sptext);
            WiFi.reconnect();
            break;
        case ARDUINO_EVENT_WIFI_STA_AUTHMODE_CHANGE:
            Tekstprintln("Authentication mode of access point has changed");
            break;
        case ARDUINO_EVENT_WIFI_STA_GOT_IP:
            sprintf(sptext, "Obtained IP address: %d.%d.%d.%d", WiFi.localIP()[0], WiFi.localIP()[1], WiFi.localIP()[2], WiFi.localIP()[3] );
            Tekstprintln(sptext);
            WiFiGotIP(event,info);
            break;
        case ARDUINO_EVENT_WIFI_STA_LOST_IP:
            Tekstprintln("Lost IP address and IP address is reset to 0");
            break;
        case ARDUINO_EVENT_WPS_ER_SUCCESS:
            txtstr = WiFi.SSID().c_str();
            sprintf(sptext, "WPS Successfull, stopping WPS and connecting to: %s: ", txtstr);
            Tekstprintln(sptext);
//            wpsStop();
//            delay(10);
//            WiFi.begin();
            break;
        case ARDUINO_EVENT_WPS_ER_FAILED:
            Tekstprintln("WPS Failed, retrying");
//            wpsStop();
//            wpsStart();
            break;
        case ARDUINO_EVENT_WPS_ER_TIMEOUT:
            Tekstprintln("WPS Timedout, retrying");
//            wpsStop();
//            wpsStart();
            break;
        case ARDUINO_EVENT_WPS_ER_PIN:
//            txtstr = wpspin2string(info.wps_er_pin.pin_code).c_str();
//            sprintf(sptext,"WPS_PIN = %s",txtstr);
//            Tekstprintln(sptext);
            break;
        case ARDUINO_EVENT_WIFI_AP_START:
            Tekstprintln("WiFi access point started");
            break;
        case ARDUINO_EVENT_WIFI_AP_STOP:
            Tekstprintln("WiFi access point  stopped");
            break;
        case ARDUINO_EVENT_WIFI_AP_STACONNECTED:
            Tekstprintln("Client connected");
            break;
        case ARDUINO_EVENT_WIFI_AP_STADISCONNECTED:
            sprintf(sptext,"Client disconnected.");                                            // Reason: %d",info.wifi_ap_stadisconnected.reason); 
            Tekstprintln(sptext);

//          Serial.print("WiFi lost connection. Reason: ");
//          Tekstprintln(info.wifi_sta_disconnected.reason);
            break;
        case ARDUINO_EVENT_WIFI_AP_STAIPASSIGNED:
            Tekstprintln("Assigned IP address to client");
            break;
        case ARDUINO_EVENT_WIFI_AP_PROBEREQRECVED:
            Tekstprintln("Received probe request");
            break;
        case ARDUINO_EVENT_WIFI_AP_GOT_IP6:
            Tekstprintln("AP IPv6 is preferred");
            break;
        case ARDUINO_EVENT_WIFI_STA_GOT_IP6:
            Tekstprintln("STA IPv6 is preferred");
            break;
        case ARDUINO_EVENT_ETH_GOT_IP6:
            Tekstprintln("Ethernet IPv6 is preferred");
            break;
        case ARDUINO_EVENT_ETH_START:
            Tekstprintln("Ethernet started");
            break;
        case ARDUINO_EVENT_ETH_STOP:
            Tekstprintln("Ethernet stopped");
            break;
        case ARDUINO_EVENT_ETH_CONNECTED:
            Tekstprintln("Ethernet connected");
            break;
        case ARDUINO_EVENT_ETH_DISCONNECTED:
            Tekstprintln("Ethernet disconnected");
            break;
        case ARDUINO_EVENT_ETH_GOT_IP:
            Tekstprintln("Obtained IP address");
            WiFiGotIP(event,info);
            break;
        default: break;
    }
}
//--------------------------------------------                                                //
// WIFI GOT IP address 
//--------------------------------------------
void WiFiGotIP(WiFiEvent_t event, WiFiEventInfo_t info)
{
  WIFIConnected = true;
//       Displayprintln("WIFI is On"); 
 if(Mem.WIFIOn) WebPage();                                                                 // Show the web page if WIFI is on
 if(Mem.NTPOn)
   {
    NTP.setTimeZone(Mem.Timezone);                                                            // TZ_Europe_Amsterdam); //\TZ_Etc_GMTp1); // TZ_Etc_UTC 
    NTP.begin();                                                                              // https://raw.githubusercontent.com/nayarsystems/posix_tz_db/master/zones.csv
    Tekstprintln("NTP is On"); 
    // Displayprintln("NTP is On");                                                           // Print the text on the display
   } 
}
//--------------------------------------------                                                //
// WIFI Check for WIFI Network 
// Check if WIFI network to connect to is available
//--------------------------------------------
 bool CheckforWIFINetwork(void)         { return CheckforWIFINetwork(true);}
 bool CheckforWIFINetwork(bool PrintIt)
 {
  WiFi.mode(WIFI_STA);                                                                         // Set WiFi to station mode and disconnect from an AP if it was previously connected.
  WiFi.disconnect(); 
  WIFIConnected = false;
  int n = WiFi.scanNetworks();                                                                // WiFi.scanNetworks will return the number of networks found
  SSIDandNetworkfound = false;
  Tekstprintln("Scanning for networks");
  if (n == 0) { Tekstprintln("no networks found"); return false;} 
  else 
    { 
     sprintf(sptext,"%d: networks found",n);    Tekstprintln(sptext);
     for (int i = 0; i < n; ++i)                                                              // Print SSID and RSSI for each network found
      {
        sprintf(sptext,"%2d: %20s %3ld %1s",i+1,WiFi.SSID(i).c_str(),WiFi.RSSI(i),(WiFi.encryptionType(i) == WIFI_AUTH_OPEN)?" ":"*");
        if (strcmp(WiFi.SSID(i).c_str(), Mem.SSID)==0) 
           {
            strcat(sptext, " -- Will connect to");
            SSIDandNetworkfound = true;
           }
        if(PrintIt) Tekstprintln(sptext);
      }
    }
 return SSIDandNetworkfound; 
 }

//--------------------------------------------                                                //
// WIFI Scan for WIFI stations
//--------------------------------------------
void ScanWIFI(void)
{
 WiFi.mode(WIFI_STA);                                                                         // Set WiFi to station mode and disconnect from an AP if it was previously connected.
 WiFi.disconnect();
 WIFIConnected = false;
 delay(100);
 int n = WiFi.scanNetworks();                                                                 // WiFi.scanNetworks will return the number of networks found.
 if (n == 0)  { Tekstprintln("no networks found");  } 
 else 
   {
    sprintf(sptext,"%d networks found",n);   Tekstprintln(sptext);
    Tekstprintln("Nr | SSID                             | RSSI | CH | Encryption");
    for(int i = 0; i < n; ++i) 
      {
       sprintf(sptext,"%2ld | %-32.32s | %4d | %2ld | ",i + 1, 
                       WiFi.SSID(i).c_str(), WiFi.RSSI(i), WiFi.channel(i));                 // Print SSID and RSSI for each network found
       Tekstprint(sptext);
       switch (WiFi.encryptionType(i))
           {
            case WIFI_AUTH_OPEN:
                Tekstprint("open");
                break;
            case WIFI_AUTH_WEP:
                Tekstprint("WEP");
                break;
            case WIFI_AUTH_WPA_PSK:
                Tekstprint("WPA");
                break;
            case WIFI_AUTH_WPA2_PSK:
                Tekstprint("WPA2");
                break;
            case WIFI_AUTH_WPA_WPA2_PSK:
                Tekstprint("WPA+WPA2");
                break;
            case WIFI_AUTH_WPA2_ENTERPRISE:
                Tekstprint("WPA2-EAP");
                break;
            case WIFI_AUTH_WPA3_PSK:
                Tekstprint("WPA3");
                break;
            case WIFI_AUTH_WPA2_WPA3_PSK:
                Tekstprint("WPA2+WPA3");
                break;
            case WIFI_AUTH_WAPI_PSK:
                Tekstprint("WAPI");
                break;
            default:
                Tekstprint("unknown");
            }
            Tekstprintln("");
            delay(10);
        }
   }
Tekstprintln("");
WiFi.scanDelete();                                                                            // Delete the scan result to free memory for code below.
}

//--------------------------------------------                                                //
// WIFI WEBPAGE 
//--------------------------------------------
void StartWIFI_NTP(void)
{
 WiFi.mode(WIFI_STA);
 WiFi.begin(Mem.SSID, Mem.Password);
 if (WiFi.waitForConnectResult() != WL_CONNECTED) 
      { 
       if(Mem.WIFINoOfRestarts == 0)                                                          // Try once to restart the ESP and make a new WIFI connection
       {
          Mem.WIFINoOfRestarts = 1;
          StoreStructInFlashMemory();                                                         // Update Mem struct   
          ESP.restart(); 
       }
       else
       {
         Mem.WIFINoOfRestarts = 0;
         StoreStructInFlashMemory();                                                          // Update Mem struct   
         // Displayprintln("WiFi Failed!");                                                   // Print the text on the display
         Tekstprintln("WiFi Failed! Enter @ to retry"); 
         WIFIConnected = 0;       
         return;
       }
      }
 else 
      {
       Tekstprint("Web page started\n");
       sprintf(sptext, "IP Address: %d.%d.%d.%d", WiFi.localIP()[0], WiFi.localIP()[1], WiFi.localIP()[2], WiFi.localIP()[3] );
       // Displayprintln(sptext);
       Tekstprintln(sptext); 
       WIFIConnected = 1;
       Mem.WIFINoOfRestarts = 0;
       StoreStructInFlashMemory();                                                            // Update Mem struct   
       // Displayprintln("WIFI is On"); 
       // AsyncElegantOTA.begin(&server);                                                     // Start ElegantOTA  old version
       ElegantOTA.begin(&server);                                                             // Start ElegantOTA  new version in 2023  
       // if compile error see here :https://docs.elegantota.pro/async-mode/
                                                                                              // Open ElegantOTA folder and then open src folder
                                                                                              // Locate the ELEGANTOTA_USE_ASYNC_WEBSERVER macro in the ElegantOTA.h file, and set it to 1:
                                                                                              // #define ELEGANTOTA_USE_ASYNC_WEBSERVER 1
                                                                                              // Save the changes to the ElegantOTA.h file.   
       if(Mem.NTPOn)
          {
           NTP.setTimeZone(Mem.Timezone);                                                     // TZ_Europe_Amsterdam); //\TZ_Etc_GMTp1); // TZ_Etc_UTC 
           NTP.begin();                                                                       // https://raw.githubusercontent.com/nayarsystems/posix_tz_db/master/zones.csv
           Tekstprintln("NTP is On"); 
           // Displayprintln("NTP is On");                                                       // Print the text on the display
          }
//       PrintIPaddressInScreen();
      }
 if(Mem.WIFIOn) WebPage();                                                                 // Show the web page if WIFI is on
}


//--------------------------------------------                                                //
// WIFI WEBPAGE 
//--------------------------------------------
void WebPage(void) 
{
 server.on("/", HTTP_GET, [](AsyncWebServerRequest *request)                                  // Send web page with input fields to client
          { request->send(200, "text/html", index_html);  }    );
 server.on("/get", HTTP_GET, [] (AsyncWebServerRequest *request)                              // Send a GET request to <ESP_IP>/get?input1=<inputMessage>
       { 
        String inputMessage;    String inputParam;
        if (request->hasParam(PARAM_INPUT_1))                                                 // GET input1 value on <ESP_IP>/get?input1=<inputMessage>
           {
            inputMessage = request->getParam(PARAM_INPUT_1)->value();
            inputParam = PARAM_INPUT_1;
           }
        else 
           {
            inputMessage = "";    //inputMessage = "No message sent";
            inputParam = "none";
           }  
  //  error oplossen       sprintf(sptext,"%s",inputMessage);    Tekstprintln(sptext); //format '%s' expects argument of type 'char*', but argument 3 has type 'String' [-Werror=format=]

        ReworkInputString(inputMessage+"\n");
        request->send(200, "text/html", index_html);
       }   );
 server.onNotFound(notFound);
 server.begin();
}

//--------------------------------------------                                                //
// WIFI WEBPAGE 
//--------------------------------------------
void notFound(AsyncWebServerRequest *request) 
{
  request->send(404, "text/plain", "Not found");
}

//                                                                                            //
//--------------------------------------------
// FIBONACCI Set the color and strip based on the time
//--------------------------------------------
void LEDsetTime(byte hours, byte minutes)
{ 
 hours %=12;                                                       // Keep the hours between 0 and 12
 for(int i=0; i<CLOCK_PIXELS; i++) { bits[i] = 0; BitSet[i] = 0; } // Clear all bits  
 MakeFibonacciList(hours);
 for(int i=0; i<CLOCK_PIXELS; i++) if(BitSet[i]) bits[i] +=1;      // If hour must be lit add 1
 for(int i=0; i<CLOCK_PIXELS; i++)    BitSet[i] = 0;               // Clear  bits  
 MakeFibonacciList(minutes/5);                                     // Block is 5 minutes  
  for(int i=0; i<CLOCK_PIXELS; i++)  
 {
   if( BitSet[i]) bits[i] +=2;    
   setPixel(i, colors[Mem.DisplayChoice][bits[i]]); 
  // Serial.print(hours); Serial.print(":"); Serial.print(minutes);  Serial.print(" ");  
  // Serial.println(colors[Mem.DisplayChoice][bits[i]],HEX);
  }
}

//--------------------------------------------
// FIBONACCI Calculate the proper Fibonacci-numbers (Pixels)
//--------------------------------------------
void MakeFibonacciList(byte Value)
{
 byte CalcValue = 0;
 byte pos = CLOCK_PIXELS;
 while (Value != CalcValue  )
  {
   byte Fibonaccireeks[] = {1,1,2,3,5,0};                          // Set up Fibonacci array with 6 numbers.
   for(int i=0; i<=CLOCK_PIXELS; i++) BitSet[i] = 0;               // Clear all bits. NB CLOCK_PIXELS is noof cells / strips in the clock
   CalcValue = 0;
   while ( (Value != CalcValue) &&  CalcValue <=  Value)   
    {
     do { pos = random(CLOCK_PIXELS); } while(Fibonaccireeks[pos] == 0 );   
     CalcValue += Fibonaccireeks[pos];
     BitSet[pos] = 1;                                              // Set pos in array for valid number    
     Fibonaccireeks[pos] = 0;                                      // Number taken from array 
    }
  }
}

//--------------------------------------------
// FIBONACCI Calculate the proper chronological numbers (Pixels)
//--------------------------------------------
void MakeChronoList(byte Hours, byte Minutes,byte Seconds)
{
 Hours %=12;                                                       // Keep the hours between 0 and 12
 byte Secsegment = Seconds / 5;
 byte Minsegment = Minutes / 5;
 byte Bit;
 uint32_t Kleur;                                                   // Color
 for(int i=0; i<12; i++)
  {
   Bit = 0;
   if(i < Hours)        Bit+= 1;                                   // If hours use the second colour
   if(i < Minsegment)   Bit+= 2;                                   // If minute use the third colour. If hours was set the fourth colour is displayed 
   if(Mem.NoExUl>0 && i == Secsegment)  Bit = 4;                   // If second use the fifth colour to display 
   Kleur = colors[Mem.DisplayChoice][Bit];
   if(Mem.NoExUl>1) {if(i<Minutes%5 && Seconds%5<1) Kleur=purple;} // If in Ultimate mode
   ColorLed(i,Kleur); 
   }
}
//--------------------------------------------
// FIBONACCI Turn on the right pixels and colours for 24 hour 
//--------------------------------------------
void setPixel(byte pixel, uint32_t kleur)
{

  switch(pixel)                                                    // 14 LEDs    
  {
   case 0:      ColorLeds("", 0, 0,kleur); break;
   case 1:      ColorLeds("", 1, 1,kleur); break;
   case 2:      ColorLeds("", 2, 3,kleur); break;
   case 3:      ColorLeds("", 4, 7,kleur); break;
   case 4:      ColorLeds("", 8,13,kleur); break;
  }

 /*
  switch(pixel)                                                    // 24 LEDs    
  {
   case 0:      ColorLeds("", 0, 1,kleur); break;
   case 1:      ColorLeds("", 2, 3,kleur); break;
   case 2:      ColorLeds("", 4, 7,kleur); break;
   case 3:      ColorLeds("", 8, 13,kleur); break;
   case 4:      ColorLeds("", 14,23,kleur); break;
  }
*/

/*
 switch(pixel)                                                     // 12 LEDs 
  {
   case 0:      ColorLeds("", 0, 0,kleur); break;
   case 1:      ColorLeds("", 1, 1,kleur); break;
   case 2:      ColorLeds("", 2, 3,kleur); break;
   case 3:      ColorLeds("", 4, 6,kleur); break;
   case 4:      ColorLeds("", 7,11,kleur); break;
  }

                                                                  // For 50x50 cm case with 174 LEDs    
 switch(pixel)  
  {
   case 0:      ColorLeds("",  0,   15,kleur); break;
   case 1:      ColorLeds("", 16,   31,kleur); break;
   case 2:      ColorLeds("", 32,   63,kleur); break;
   case 3:      ColorLeds("", 64,  103,kleur); break;
   case 4:      ColorLeds("", 104, 173,kleur); break; 
  }
                                                                   // For case with 3 x 12 LEDs clock 
  switch(pixel)  
  {
   case 0:      ColorLeds("", 0,   2,kleur); break;
   case 1:      ColorLeds("", 3,   5,kleur); break;
   case 2:      ColorLeds("", 6,  11,kleur); break;
   case 3:      ColorLeds("", 12, 20,kleur); break;
   case 4:      ColorLeds("", 21, 35,kleur); break; 
  }

                                                                       // For case with clock 
   switch(pixel)  
  {
   case 0:      ColorLeds("", 0,   2,kleur); break;
   case 1:      ColorLeds("", 3,   5,kleur); break;
   case 2:      ColorLeds("", 6,  11,kleur); break;
   case 3:      ColorLeds("", 12, 20,kleur); break;
   case 4:      ColorLeds("", 21, 35,kleur); break; 
  }

  switch(pixel)                                                     // for 32 LEDs, 4 strips of 8 LEDs
   {
    case 0:      ColorLeds("", 2, 3,kleur);                            break;
    case 1:      ColorLeds("",12,13,kleur);                            break;
    case 2:      ColorLeds("", 0, 1,kleur); ColorLeds("",14,15,kleur); break;
    case 3:      ColorLeds("",16,19,kleur); ColorLeds("",28,31,kleur); break;
    case 4:      ColorLeds("", 4,11,kleur); ColorLeds("",20,27,kleur); break;
   }
 */  
}
