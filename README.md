# Welcome to our Cansat repository

There are 2 folders, one for the arduino code and one for the python code, inside the arduino folder, there are 2 more seperate folders with groundstation and cansat codes and instructions on how to use them.

## What is Cansat

Cansat is a competition where you have to design a little satellite that will go 1km into the air by rocket and you have to receive some data from it and hopefully get it recovered.

## Parts used for our Cansat

- Arduino Nano microcontroller.
- Adafruit bmp 280 temperature and pressure sensor.
- U-blox neo-7m gps module.
- A cheap micro sd card reader.
- Ebyte SX1278 100mW E32-433T20DT UART LoRa transceiver.
- A 2dBi SMA omnidirectional 433MHz antenna.
- A buck-converter set to ±3.3V.
- 2x 3.7V LiPo single cel batteries in parallel.
- A master switch between batteries and all the parts
- A 3d printed inner and outer case.
- A parachute made from neon pink 100% polyester.

## Wiring Cansat

![image](https://user-images.githubusercontent.com/25268098/122579622-5f9f3900-d055-11eb-8cfe-82c8ba8206b7.png)

Made with EasyEDA

## Parts used for our groundstation

- Arduino Uno microcontroller.
- Ebyte SX1278 100mW E32-433T20DT UART LoRa transceiver.
- A 2-3 dBi 50ohm omnidirectional 433MHz antenna.

## Programs used to graph the data

- CoolTermWin for writing the received data to a text file.
- Python for reading that file and plotting the data into nice graphs.
