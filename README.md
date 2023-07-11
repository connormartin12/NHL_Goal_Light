# NHL_Goal_Light

The goal of this project is to create a device with the capability of collecting and displaying the score of an ongoing NHL game. The device will play a goal horn sound, set off a goal light, and supply text output on an OLED screen as the user's team scores in real time.

To achieve this, the project has been divided into two main parts: The ESP32 and the mobile app.

## ESP32

The esp32 will scrape the NHL score api and contain the logic to control the above outcomes. It will also have bluetooth functionality to receive Wi-Fi credentials from the user as well as other customized inputs.

## Mobile App

A mobile app, developed with react-native, will enable the user to connect to the esp via bluetooth to send relative information (such as Wi-Fi credentials and NHL team selection) to the esp. The mobile app will also enable OTA (over the air) updates to the esp32 program to keep the product up to date and fix any bugs found.