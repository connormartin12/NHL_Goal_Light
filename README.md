# NHL_Goal_Light

## About This Project

The goal of this project is to create a device with the capability of collecting and displaying the score of an ongoing NHL game which is determined by the user. The device will play a sound, set off a goal light, and supply text output on an OLED screen when the user's team scores in real time.

This project is divided in to two main parts:

### ESP32

The esp32 will scrape the NHL score api and contain the logic to control the above outcomes. It will also have bluetooth functionality to receive Wi-Fi credentials from the user as well as other customized inputs.

### Mobile App

A mobile app, developed with react-native, will enable the user to connect to the esp via bluetooth to send relative information (such as Wi-Fi credentials and NHL team selection) to the esp. The mobile app will also enable OTA (over the air) updates to the esp32 program to keep the product up to date and fix any bugs found.