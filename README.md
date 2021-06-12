# Welcome to our Cansat repository

There are 2 folders, one for the arduino code and one for the python code, inside the arduino folder, there are 2 more seperate folders with groundstation and cansat codes and instructions on how to use them.

## What is Cansat

Cansat is a competition where you have to design a little satellite that will go 1km into the air with a rocket and you have to receive some data from it and hopefully get it recovered.

## Parts used Cansat:

- Arduino Nano microcontroller.
- Adafruit bmp 280 temperature and pressure sensor.
- U-blox neo-7m gps module.
- A cheap micro sd card reader.
- Shiwaki 100mW E32-433T20DT UART LoRa transceiver.
- A buck-converter set to 3.3V.
- 2 3.7 LiPo 1 cel batteries in parallel.
- A 3d printed inner and outer case.

## Parts used groundstation

- Arduino Uno microcontroller.
- Shiwaki 100mW E32-433T20DT UART LoRa transceiver.

## Programs used to graph the data

- CoolTermWin for writing the received data to a text file.
- Python for reading that file and plotting the data into nice graphs.
