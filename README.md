# Fibonacci-ESP32-C3 or S3 Clock
Fibonacci clock with an ESP32 C3 or S3

![PrototypeS3](https://github.com/ednieuw/Fibonacci-ESP32-C3-S3-Clock/assets/12166816/cade51cf-ec1c-4248-b3cf-6d65ba484731)
Prototype

A clock that displays time in colours.  
![IMG_6128](https://github.com/ednieuw/Fibonacci-ESP32-C3-S3-Clock/assets/12166816/e79ee14f-d8e0-469f-83f9-d1d13a5135cf)


Time is synchronized with the Network Time Protocol (NTP) from the internet. 
Settings can be controlled with a webpage, a PC or a Bluetooth Low Energy (BLE) serial terminal app installed on a phone, PC or tablet.

This clock is built with a ESP32-C3-supermini and an ESP32-S3-zero with Espressif BLE nRF52 and WIFI. 
It is possible to use any ESP32 as long as the used pins are adapted in the program.  
Soldering pins to the ESP32-C3-supermini was difficult. Even with Pb/Sn solder at 250 Celsius often the board was not function anymore because SMD’s came loose from the PCB or were destroyed. I ordered low melting solder paste 
I used the ESP32-S3-zero with welded pins. 
Also the use of the internal LED must be adapted to the board that is used or turn off. Some boards use RGB LEDs connected to three pins and other board use a WS2812 RGB LED.  
![IMG_7943](https://github.com/ednieuw/Fibonacci-ESP32-C3-S3-Clock/assets/12166816/984d59dd-759a-4b8a-9a5d-9a50959e1591)

# Before starting

The clock receives time from the internet. Therefore the name of the WIFI station and its password must be entered to be able to connect to a WIFI router.
The name of the WIFI-station and password has to be entered once. These credentials will be stored in memory of the MCU.
To make life easy it is preferred to use a phone and an communication app of a phone or tablet enter the WIFI credentials into the clock.
 	 	 
BLESerial nRF	BLE Serial Pro	Serial Bluetooth Terminal
- Download a Bluetooth UART serial terminal app on your phone, PC, or tablet.
- For IOS: BLE Serial Pro or BLESerial nRF.
- For Android: Serial Bluetooth Terminal.

# Compiling the program

```
//========================================================================================
//#define ESP32C3_SUPERMINI
#define ESP32S3_ZERO

// ------------------>   Define only one type of LED strip 
#define LED2812
//#define LED6812            // Choose between LED type RGB=LED2812 or RGBW=LED6812
// --------------->   Define only one library

// --------------->   Define How many LEDs in fibonacci clock
const int NUM_LEDS = 14;     // How many leds in fibonacci clock?
                             // If Not 14 LEDs --->  Update also LED positions in setPixel()
                             //    (at the end of file) TOO!  

//------------------------------------------------------------------------------
// PIN Assignments for Arduino Nano ESP32
//------------------------------------------------------------------------------ 

                         #ifdef ESP32C3_SUPERMINI
#define  PhotoCellPin 0
#define  LED_PIN      4       // D5 Pin to control colour SK6812/WS2812 LEDs
#define  secondsPin   3       // If LED is connected to Pin IO3 with 470 ohm resistor
                         #endif // ESP32C3_SUPERMINI

                         #ifdef ESP32S3_ZERO
#define  PhotoCellPin 2
#define  LED_PIN      1       // D5 Pin to control colour SK6812/WS2812 LEDs
#define  secondsPin   3       // If LED is connected to Pin IO3 with 470 ohm resistor
                         #endif // ESP32S3_ZERO
```
At the top of the INO-file one has to define what is installed. 
In the example above the ESP32S3_ZERO with WS2812 LEDS are used.
A few line lower the pin definitions for the attached LDR and LED-strip are defined.

The working of the code is explained here: https://github.com/ednieuw/ESP32-C3-Clock-and-more
Building the Fibonacci clock here: https://github.com/ednieuw/Fibonacci-Vierkantekokerklok
The case can also be 3d-printed. See here:  https://www.thingiverse.com/thing:6483258
And here: https://ednieuw.home.xs4all.nl/Woordklok/FibonacciClock/FibonacciClock.html

