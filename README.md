# TPL-II
## Taupunktlüftung II => dewpoint ventilation II
Description in progress....

The original project Taupunktlüftung is created by ct/Heise.

I extended and modified the code for some additional functions and some optional hardware has been added, too.


## purpose, just in case, you don't know...



## Hardware-Overview

the project hardware is based on some separate modules.
1. mainboard
2. power supply
3. relais board
4. I2C display

### the mainboard
the mainboard is developed by myself using KiCAD software. it has sockets for the Arduino nano, which is obligatory and for an ESP01 or Wemos D1. the source code published here is developed with Arduino IDE and has to be uploaded to the Atmega 328P chip with new bootloader. you can add an ESP8266 (wemos D1 mini pro or ESP01) to this system. this additional module is intendet to be programmed with Tasmota firmware. It reads serial data from the Arduino board an can deliver the data to an MQTT server. With serialsend you can use the ESP chip, so send serial data to the Arduino, for example, to initiate a button press.


### power supply

Switching power supply, board creaded with KiCAD. Output is 5V 10W (or more).


### relais board

2 relais baord from a chinese manufacturer. be sure to buy good quality with air gap between the contacts.


### I2C display

LCD background lit text display with 2 x 40 characters HD44780 compatible and I2C interface
