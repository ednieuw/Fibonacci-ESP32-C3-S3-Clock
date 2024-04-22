# Fibonacci-ESP32-C3 or S3 Clock
Fibonacci clock with an ESP32 C3 or S3 (text below is in developement phase )

![PrototypeS3](https://github.com/ednieuw/Fibonacci-ESP32-C3-S3-Clock/assets/12166816/cade51cf-ec1c-4248-b3cf-6d65ba484731)
Prototype

A clock that displays time in colours.  
![IMG_6128](https://github.com/ednieuw/Fibonacci-ESP32-C3-S3-Clock/assets/12166816/e79ee14f-d8e0-469f-83f9-d1d13a5135cf)


Time is synchronized with the Network Time Protocol (NTP) from the internet. 
Settings can be controlled with a webpage, a PC or a Bluetooth Low Energy (BLE) serial terminal app installed on a phone, PC or tablet.

This clock is built with a ESP32-C3-supermini and an ESP32-S3-zero with Espressif BLE nRF52 and WIFI. 
It is possible to use any ESP32 as long as the used pins are adapted in the program.  
![S3_C3_MCU](https://github.com/ednieuw/Fibonacci-ESP32-C3-S3-Clock/assets/12166816/3b6b667f-b216-47f9-908e-f0fc4708580d)

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

- The working of the code is explained here: https://github.com/ednieuw/ESP32-C3-Clock-and-more
- Building the Fibonacci clock here: https://github.com/ednieuw/Fibonacci-Vierkantekokerklok
- The case can also be 3d-printed. See here:  https://www.thingiverse.com/thing:6483258
- And here: https://ednieuw.home.xs4all.nl/Woordklok/FibonacciClock/FibonacciClock.html

Connect the parts as below for the ESP32-C3-supermini
![ESP32C3SuperMini-FibonacciClock-V03](https://github.com/ednieuw/Fibonacci-ESP32-C3-S3-Clock/assets/12166816/afda037b-d755-4bfd-b77c-a0076784dd04) 

Connect the parts as below for the Waveshare ESP32-S3-Zero. 
Keep in mind that the pin layout can be different for other ESP32-S3-Zero's. 
![ESP32S3-Welded-Zero-FibonacciClock-V02](https://github.com/ednieuw/Fibonacci-ESP32-C3-S3-Clock/assets/12166816/bd49306a-7c72-49d1-a5b9-c9870e786b95)

# Using OTA
An easy method is to install the Fibonacci clock program for the first time Over The Air (OTA)
In my program I use the ElegantOTA library but I found out recently that the OTA from Arduino also is also fine to use.

Open in Examples in the Arduino IDE: ArduinoOTA 
Enter your WIFI credentials in the program at line 8 and 9
Compile and upload to the ESP32
Find out in your router what IP address it has received.
Type in a  browser the IP-address. Something like http://192.168.178.156/ in my case.
Login with ‘’admin’’ and ‘’admin’’
Load the “ESP32-C3S3-FiboV006.ino.esp32s3.bin” file. (or a higher V006) 
And hops, the app is running.
Enter your WIFI credentials with BLE op your mobile phone in the menu of the clock, Reset and the clock is running.


 


#  Installations  

To connect to a WIFI network a SSID (WIFI name)  and password must be entered.
There are a few methods:
Connect the MCU in the clock with a micro USB serial cable to a PC and use a serial terminal.  
Use a BLE serial terminal app on a phone or tablet for connection.

For a PC the app Termite is fine as serial terminal.

For IOS use:  BLE Serial Pro or BLESerial nRF. 

For Android use: Serial Bluetooth terminal. 
![image](https://github.com/ednieuw/Fibonacci-ESP32-C3-S3-Clock/assets/12166816/7b269328-cea3-433c-bee3-04485b151789)

Bluetooth Low Energy (BLE) can use two types of protocol CC25nn or nRF52nn where nn is a specific number. This clock uses nRF52 from the company Nordic.  

- Start the app and start a connection with the clock. Some apps automatically start with a connection window but for some a connection symbol must be pressed. You will most probably find one station to select from. 

- Select the clock in the list.

- The app will display a window and a line where commands can be entered and send to the clock.

- Sending the letter I or i for information will display the menu followed with the actual settings of several preferences.
On a ESP32-S3 there is a LED that will have a red dot lighted when the program is running. 
A green dot will turn on when there is a WIFI connection.
When there is a Bluetooth connection a blue dot in the LED will light.

In both cases send the letter I of Information and the menu shows up.
Enter the first letter of the setting you want to changes followed with a code.
Some entries just toggle On and Off. Like the W to set WIFI Off or On. 
To change the SSID and password:
Send the letter A or a followed with the WIFI station name. 
Amy-ssid and send this command. Eg AFRITZ!Box01 or aFRITZ!Box01. Starting with an upper or lower case character is an identical instruction in the command string
Then the letter B followed with the password.
Bmypassword and send the password.
Cbroadcastname  will change to name displayed in the Bluetooth connection list. Something like: cMyClock

If the length of the SSID and/or password is less then 5 characters the WIFI will be turned off automatically to avoid connection errors.
Use a length of minimal 8 characters for SSID and password.
Check in the menu (third row from the bottom) if WIFI and NTP are on.
![image](https://github.com/ednieuw/Fibonacci-ESP32-C3-S3-Clock/assets/12166816/2eeb1005-affa-4c01-b7c8-1cc9cb60b0be)

Enter @ to restart the MCU. It will restart and connections will be made. 
Sometimes a second or third reset must be given to get the clock connected to WIFI. If connection still fails check the SSID name and the entered password. (send the letter b, an easter egg))
If WIFI is connected the LED on the MCU will turn on a green dot.	
```
___________________________________
A SSID B Password C BLE beacon name
D Date (D15012021) T Time (T132145)
E Timezone  (E<-02>2 or E<+01>-1)
  Make own colour of:  (Hex RRGGBB)
F Font  G Dimmed font H Bkgnd
I To print this Info menu
K LDR reads/sec toggle On/Off
L L0 = NL, L1 = UK, L2 = DE
  L3 = FR, L4 = Wheel
N Display off between Nhhhh (N2208)
O Display toggle On/Off
P Status LED toggle On/Off
Q Display colour choice      (Q0-7)
  Q0 Yellow  Q1 hourly
  Q2 White   Q3 All Own
  Q4 Own     Q5 Wheel
  Q6 Digital Q7 Analog display
R Reset settings @ = Reset MCU
--Light intensity settings (1-250)--
S=Slope V=Min  U=Max   (S80 V5 U200)
W=WIFI  X=NTP& Y=BLE  Z=Fast BLE
Ed Nieuwenhuys Aug 2023
___________________________________
Display off between: 00h - 00h
Display choice: Yellow
Slope: 80     Min: 5     Max: 255 
SSID: FRITZ!BoxEd
BLE name: PaarseS3N8R2
IP-address: 192.168.178.106 (/update)
Timezone:CET-1CEST,M3.5.0,M10.5.0/3
WIFI=On NTP=On BLE=On FastBLE=Off
Language choice: Rotate language
Software: ESP32C3S3_WordClockV025.ino
02/09/2023 21:06:27 
___________________________________
```
 	Menu displayed in serial output.

To set a time zone. Send the time zone string between the quotes prefixed with the character E or e.
See the entry strings for the time zones at the bottom of this page.
For example; if you live in Australia/Sydney send the string, eAEST-10AEDT,M10.1.0,M4.1.0/3


![image](https://github.com/ednieuw/Fibonacci-ESP32-C3-S3-Clock/assets/12166816/8daaab6c-dc05-4f5d-90a4-b1587850bb27) 
HTML page on iPhone 	

![image](https://github.com/ednieuw/Fibonacci-ESP32-C3-S3-Clock/assets/12166816/83b67d80-39b5-448c-a5af-fd27c681005d)
Termite Terminal from a PC

# Upgrading software
Software can be upgraded over the air. (OTA) by opening a web browser and entering the IP-address of the clock followed with /update. 
For example: 192.168.178.78/update. 
Choose firmware and click on Choose File.
Choose the appropriate bin file. 

# Control and settings of the clock
As mentioned before the clock can be controlled with the WIFI webpage or BLE UART terminal app.
When the clock is connected to WIFI the IP-address is displayed when the clock is in Digital display mode. (Q6 in the menu)
The IP-address is also printed in the menu. As a last resort the IP-address can be found in you WIFI router. 

To start the menu in a web page the IP-address numbers and dots (for example: 192.168.178.77) must be entered in the web browser of your mobile or PC where you type your internet addresses (URL).

Or with a Bluetooth connection:

Open the BLE terminal app. 
Look for the clock to connect to and connect.

Unfortunately some apps can not read strings longer than 20 characters and you will see the strings truncated or garbled.
If you see a garbled menu enter and send the character 'Z' to select the slower transmission mode.
If transmission is too garbled and it is impossible to send the character Z try the web page to send the character Z.
A third method is to use an iPhone, iPad of iMac with the free BLE nRF app.
If all fails you have to connect the MCU inside the clock  with a micro USB cable to a PC and use a serial terminal app to send a Z.
 

# Regulating the light intensity of the display

In the menu light intensity of the display can be controlled with three parameters:
--Light intensity settings (1-250)--
S=Slope V=Min  U=Max   (S80 V5 U200)
The default values are between the ().

S How fast the brightness reaches maximum brightness.
V How bright the display is in complete darkness.
U the maximum brightness of the display.

In the bottom half of the menu the stored values are displayed
Slope: 80     Min: 5     Max: 255

The clock reacts on light with its LDR (light dependent resistor).


When it gets dark the display does not turn off completely but will stay dimmed at a minimum value. 
With the parameter V the lowest brightness can be controlled. With a value between 0 and 255 this brightness can be set.
V5 is the default value. 
The maximum brightness of the display is controlled with the parameter U. Also a value between 0 and 255.
With parameter S the slope can be controlled how fast maximum brightness is achieved. 



 
Settings are set by entering the first character of a command following by parameters if necessary.
For example: 
To set the colours of the fonts in the display to white enter: Q2



Turn off WIFI by sending a W.
Restart the clock with the letter @.
Reset to default setting by send R. 

  	 
HTML page	BLE menu

 
# Updating the software
 
The software can be updated ‘Over The Air’ when the clock is connected to WIFI.
You can find the IP-address in the menu or in the digital display mode menu option Q6.
Enter the IP-address of the clock followed with /update
http://192.168.178.78/update

‘Choose File’ in the menu and select the bin file to update.
Something like: Liygo-WristWatchESP32-V011.ino.twatch.bin 
where V011 is the version number
 


# Detailed description

With the menu many preferences can be set. These preferences are stored on a SD-card or in the ESP32-S3 storage space.
 
Enter the first character in the menu of the item to be changed followed with the parameter.
There is no difference between upper or lower case. Both are OK.
Between the ( ).

```
A SSID B Password C BLE beacon name
Change the name of the SSID of the router to be connected to. 
For example: aFRITZ!BoxEd or AFRITZ!BoxEd.
Then enter the password. For example: BSecret_password.
Restart the clock by sending @. 
Entering a single 'b' will show the used password. This ‘Easter egg’ can be used to check if a valid password was entered.

D Set Date  and T Set Time 
If you are not connected to WIFI you have to set time and date by hand.
For example enter: D06112022 to set the date to 6 November 2022.
Enter for example T132145 (or 132145 , or t132145)  to set the time to 45 seconds and 21 minute past one o'clock.
E Set Timezone E<-02>2 or E<+01>-1
At the bottom of this page you can find the time zones used in 2022. 
It is a rather complicated string and it is therefore wise to copy it.
Let's pick one if you happen to live here: Antarctica/Troll,"<+00>0<+02>-2,M3.5.0/1,M10.5.0/3"
Copy the string between the " "'s and send it with starting with an 'E' or 'e' in front.
E<+00>0<+02>-2,M3.5.0/1,M10.5.0/3.
Time zones and daylight savings should be ended and replaced by one universal date and time for the while planet cq universe. But that is my opinion.
Make own colour of: (Hex RRGGBB)
F Font G Dimmed font H Bkgnd
You can set the colours of the highlighted and dimmed characters and the background. .
The format to be entered is hexadecimal. 0123456789ABCDEF are the characters that can be used. The command is 2 digits for Red followed with  two for Green and ending with two digits for Blue. 
To colour the characters intense red enter FF0000 prefixed with the letter F.
To set the background to intense blue enter: H0000FF
To set the dimmed character to dark grey enter for example: G191919. 
You get grey if red, green and blue has the same intensity.
I To print this Info menu
Print the menu to Bluetooth and the serial monitor connected with an USB-cable.
K Reads/sec toggle On/Off
Entering a K toggles printing of the LDR reading of the measured light intensity. 
It also shows how many times the processor loops through the program and checks its tasks to run the clock. 

L L0 = NL, L1 = UK, L2 = DE, L3 = FR, L4 = Wheel
You can choose between four languages to display or show them randomly every minute.

N Display off between Nhhhh (N2208)
With N2208 the display will be turned off between 22:00 and 8:00.

O Display toggle On/Off
O toggle the display off and on.
P Status LEDs toggle On/Off
P toggle the status LEDs on the MCU off and on.

Q Display colour choice (Q0-6)
Q0 Yellow Q1 hourly Q2 White Q3 All Own Q4 Own Q5 Wheel Q6 Digital display

Q0 will show the time with yellow words.
Q1 will show every hour another colour.
Q2 shows all the texts white.
Q3 and Q4 uses you own defined colours.
Q5 will follow rainbow colours every minute.
Q6 is the digital display with the IP-address and date until seconds are 00.
Q7 is the analogue clock
R Reset settings 
R will set all preferences to default settings, it also clears the SSID and password.
--Light intensity settings (1-250)--
S=Slope V=Min  U=Max   (S80 V5 U200)
The
@ = Reset MCU
@ will restart the MCU. This is handy when the SSID, et cetera are changed and the program must be restarted. Settings will not be deleted.
 
W=WIFI, X=NTP&, Y=BLE, Z=Use SD
Toggle WIFI, NTP on and off.
Enter the character will toggle it on or off. 
At the bottom of the menu the stated is printed.
 
Sending a & will start a query from the time server.
Z Fast BLE
The BLE UART protocol sends default packets of 20 bytes. Between every packet there is a delay of 50 msec.
The IOS BLEserial app, and maybe others too, is able to receive packets of 80 bytes or more before characters are missed. 
Option Z toggles between the long and short packages. 
```


