## Library dependencies
- EBYTE.h

## Extra information
- It is important that if you have the temperature sensor with gps working, that you use the groundstation_all.ino, because the received struct has to be the same as the send struct.
- If only the GPS and the transmitter is working (and SD card), use groundstation_gps.ino.
- You can use a program like _cooltermwin_ to log the serial monitor to a file, that you can later manipulate with python.
