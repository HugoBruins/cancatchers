# Welcome to our Cansat repository

There are 2 folders, one for the arduino code and one for the python code, inside the arduino folder, there are 2 more seperate folders with groundstation and cansat codes and instructions on how to use them.

## What is Cansat

Cansat is a competition where you have to design a little satellite that will go 1km into the air by rocket and you have to receive some data from it and hopefully get it recovered.

## Project finished

Launch date: 17-09-2021

### Received data (Launch Day)

Data Plots:
![image](https://user-images.githubusercontent.com/25268098/133812163-57eb1e61-c164-4e92-8f94-41eb4e3889fe.png)

The temperature graph is a little screwed, the wire of the temperature sensor was a bit too short so it couldn't be fitted externally like original design.

Average wind speed from start of descent: 3.1085334171646997 ms^-1.

Average descent speed:  13.19164960342676  ms^-1.

Terminal velocity:  13.096136803754872 ms^-1 (Almost too slow!)

###2d plot:

![image](https://user-images.githubusercontent.com/25268098/133815980-03e516cc-0f64-458d-9644-ecbdffdf16cd.png)

###3d plot:

![image](https://user-images.githubusercontent.com/25268098/133816271-5a08645c-8c94-4f89-ad9a-e00f418c2b4d.png)

We were missing some data points on ascent, this is because the cansat was in a metal cage and receiving data was very hard.



## Parts used for our Cansat

- Arduino Nano microcontroller.
- BMP 280 breakout temperature and pressure sensor.
- ATGM336H gps module.
- A cheap micro sd card reader.
- Ebyte SX1278 100mW E32-433T20DT UART LoRa transceiver.
- A 2dBi SMA omnidirectional 433MHz antenna.
- A buck-converter set to ±3.3V. 
- A blue LED.
- A 330Ω resistor.
- 2x 3.7V LiPo single cel 650mAh batteries in parallel.
- A master switch between batteries and all the parts
- A 3d printed inner and outer case.
- A parachute made from neon pink 100% polyester.

We had to use a buck converter to convert the 5V out from the Arduino to 3.3V, we figured out the hard way that driving all the required current from the 3.3V pin fries the 5V Arduino Nano usb-port. The GPS TX is only connected to the RX because the GPS I/O is not 3.3V compatible, plus we only need to receive the data from it, if we want to send data to it (change settings) we just need to desolder it and use a USB - TLL converter, but changing settings is not done often. The settigs we are rocking (configured using GnssToolkit3 program): 

- baud rate = 38400
- frequency = 2hz
- GPS constellations: GPS (USA), GLONASS (Russia), BDS (China)

The BMP280 is still on the +3.3V rail on the Arduino because it becomes less accurate on the buck-converter. 

We opted for a LoRa module, because the stock transmitter was a damaged module, this LoRa module has enough link budget to reach the kármán line in perfect/ideal circumstances using our setup. In a distance test we got 2 kilometers easily (almost no packet loss) with lots of stuff inside of the fresnel zone (also with the receiver inside a car). 

## Wiring Cansat and groundstation

![image](https://user-images.githubusercontent.com/25268098/123482494-8c2ef400-d605-11eb-8bf8-ac5119fdb300.png)

Made with EasyEDA

## Parts used for our groundstation

- Arduino Uno microcontroller.
- Ebyte SX1278 100mW E32-433T20DT UART LoRa transceiver.
- A 2-3 dBi 50ohm omnidirectional 433MHz antenna.

## Programs used to graph the data

- We use python for everything, in the python subdirectory another readme exists that explains more.
