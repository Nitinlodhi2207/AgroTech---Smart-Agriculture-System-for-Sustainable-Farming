# AgroTech---Smart-Agriculture-System-for-Sustainable-Farming
##Developed weather monitoring and agricultural automation  with ESP8266 microcontroller. 

## Introduction
-Here we introduce a smart weather reporting system over the Internet. Our introduced system allows for weather parameter reporting over the Internet. It allows the people to directly check the weather states online without the need of a weather forecasting agency. System uses temperature, humidity as well as rain with humidity sensor to monitor weather and provide live reporting of the weather statistics. The system constantly monitors temperature using temperature sensor, humidity using humidity sensor and also for rain. Weather monitoring system deals with detecting and gathering various weather parameters at different locations which can be analysed or used for weather forecasting. 

## Overview 
Assemble all system as per circuit diagram. Program the NodeMCU using Arduino IDE. You will get confirmation on your screen once The NodeMCU is a programable controller which has inbuilt wi-fi module. We connect three sensors 1) BMP180 2) DHT11 and 3) Rain Sensor to NodeMCU. By using these three sensors, we can collect the required weather data for monitoring purpose. This pooled data is stream over the Internet to display it or read it from anywhere. After the successfully programmed hardware, the NodeMCU get one IP address. We can browse this IP address from any of WEB browser like Chrome, Firefox, Internet Explorer etc.so we display the required live data which fetched by sensors in beautiful Graphical User Interface format. The weather parameters that we monitor are Temperature, Pressure, Humidity and Rain. Also, you can check whether data through anywhere using Internet as we hosted this server publicly. We developed an android application for easy access to our weather monitoring system.
 
## Components List and Technical Details
### NodeMCU (ESP8266)<br>
Microcontroller: Tensilica Xtensa LX106<br>
Clock Speed: 80 MHz<br>
Flash Memory: 4 MB<br>
Wi-Fi: 802.11 b/g/n<br>
Operating Voltage: 3.3V<br>
GPIO Pins: 17<br>
### BMP180 Sensor (Pressure)<br>
Pressure Range: 300 to 1100 hPa<br>
Accuracy: ±1.0 hPa<br>
Interface: I2C<br>
Operating Voltage: 1.8V to 3.6V<br>
### DHT11 Sensor (Temperature and Humidity)<br>
Temperature Range: 0 to 50 °C<br>
Temperature Accuracy: ±2 °C<br>
Humidity Range: 20% to 90% RH<br>
Humidity Accuracy: ±5% RH<br>
Interface: Single-wire digital signal<br>
Operating Voltage: 3.5V to 5.5V<br>
### Rain Sensor<br>
Output: Analog and digital<br>
Operating Voltage: 3.3V to 5V<br>
Rain Detection: Detects the presence and intensity of rain<br>
Interface: Analog pin and digital pin ( i am using Analog pins)<br>
### Breadboard and Connecting Wires<br>
Used for prototyping and making temporary connections.<br>
###Power Supply(Hi-Link HLK 10M05 5V/10W Switch Power Supply Module)<br>
Provides necessary voltage and current to the components.<br>
### Arduino IDE (Software for programming the microcontroller)<br>
here used for programming the NodeMCU.<br>
### Wi-Fi Router(I used my mobile)<br>
Provides Internet connectivity for data transmission.<br>
### Altium Designer (CAD Software for PCB Design)<br>
Used for creating circuit schematics and PCB layouts.<br>
