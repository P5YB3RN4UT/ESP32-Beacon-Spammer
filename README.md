<p align="center">
 <img src="https://www.media-underground.net/images/beacon.png">
</p>

<h3 align="center">ESP32 BEACON SPAMMER</h3>

---


## Summary
This project involves creating a portable waterproof device that uses an ESP32 microcontroller to function as a WiFi access point spammer.


## Disclaimer 
This tool is strictly for educational purposes only. The author does not take responsibility for any illegal activity undertaken from the misuse of this software.


## Introduction
This project includes source code for a Beacon Spammer with the following requirements:

- <b>Spammer Capabilities</b>

  <i>To be able to select one of ten customisable presets from a Web User Interface accessible by connecting to an initial access point being broadcast by the ESP32.</i>

- <b>Maximising Spammer Capabilities</b>

  <i>After selecting one of the presets the Web UI and initial access point should shut down in order to maximise beacon spam performance.</i>

- <b>Waterproofing</b>

  <i>The device needs to be sufficiently waterproofed in order to be deployed virtually anywhere regardless of weather conditions.</i>


## Overall Concept
The basic idea was to create a standalone 'Beacon Spammer' device so sufficiently affordable that losing one shouldn't be an issue. Most devices that can run this feature come bundled with other features (for example, the LilyGO T-Embed CC1101 or M5Stack Cardputer ADV running Bruce Firmware) where deployment and loss of the device is undesirable. Also, deployment of these other devices under different weather conditions is impractical and unrealistic, hence this standalone project.

Note: All ten presets are completely customisable at code level. Currently on this build I have created five ready-to-go presets and left five ready for customisation. The five ready-to-go presets are as follows:
1. TEST: <i>This broadcasts ten test access points.</i>
2. RICKROLL: <i>Everyone's favourite Rick Astley spamming roll of the "Never Gonna Give You Up" chorus lyrics.</i>
3. GOETIA: <i>This is one for the occultists out there and lists all 72 Ars Goetia demons from the Lesser Key of Solomon.</i>
4. GOLF: <i>I hate everything about Golf and everyone who plays this stupid game, so I created this.</i>
5. SODROLL: <i>Chorus lyrics to the song "Kill Yourself" by the band 'Stormtroopers Of Death'.</i>


## Hardware

Required:
1. ESP32-WROOM-32 (Compatible with ESP32-WROOM-32D, ESP32-WROOM-32E, ESP32-WROVER)
2. Programming Cable
3. 18650 Battery
4. 18650 Battery Holder
5. TC4056/TP4056 Charging Module
6. Waterproof Push Button Switch
7. Waterproof USB-C Socket
8. Waterproof Project Box

Additional:
1. Perfboard
2. Pin Headers
3. LEDs For Battery Charge Status
 - <i>Note: Remove and replace 1 or 2 of the charging module's surface mount LEDs with 2 pin round head LEDS if you wish to see either battery charging, battery charged, or both from the outside of the housing. Alternatively, if you're careful, you can wire the round head LEDs in parallel with the onboard surface mount LEDs.</i>


## Software

[ArduinoIDE](https://www.arduino.cc/en/software) - For Programming The Device.


## Pinout

| 18650 Battery | TC/TP4056 Charging Module | ESP32-WROOM-32 | USB-C Socket |
|:---:|:---:|:---:|:---:|
| + | B + |  |  |
| -	| B - |  |  |
|  	| OUT + | 3V3 |  |
|  	| OUT - | GND |  |
|  	| IN + |  | RED WIRE |
|  	| IN - |  | BLACK WIRE |


## Installation
Flashing the ESP32:
1. Install **Arduino IDE** and add the ESP32 board support package:
   - Open **Arduino IDE**.
   - Go to **File > Preferences**.
   - In **Additional Board Manager URLs**, add:  
       https://raw.githubusercontent.com/espressif/arduino-esp32/gh-pages/package_esp32_index.json

2. Download the **ESP32-Beacon-Spammer** Repository and unzip the contents to the Arduino folder.
   - Open **Arduino IDE** and go to **File > Open**.
   - Select the downloaded main.ino file.

3. Lines 17 to 26 of the code includes ten presets. You can modify these to whatever you want (maximum 32 characters per SSID and separated by **\n**. You can also spoof WPA2 encryption on each preset by replacing **False** with **True** at the end of each preset.

4. Line 244 of the code is the SSID and password for the initial Web UI access point. It can be changed here.
 
5. Compile and upload the code to your ESP32.


## Operation

1. Switch on the device.
2. Connect to the WiFi access point "**•°|°•**" with the password "**p5yb3rn4ut**".
3. Open a browser at the URL **http://192.168.4.1**.
4. Choose one of the ten target presets and hit "**EXECUTE**".
5. A final response will be sent before the Web UI and initial access point completely shut down to maximise beacon spam performance.
6. To choose another preset either disconnect and reconnect power to the ESP32 or hit the onboard reset button and repeat operation as before.


## Photos

<img src="https://www.media-underground.net/images/evil_portable01.jpg">
<img src="https://www.media-underground.net/images/evil_portable02.jpg">
<img src="https://www.media-underground.net/images/evil_portable03.jpg">

[![WDGWars](https://wdgwars.pl/card/345-4HN42QTP.svg)](https://wdgwars.pl/)
