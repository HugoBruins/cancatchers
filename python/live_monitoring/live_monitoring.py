# this code opens the ground station port and reads from the Arduino
# after that it will proceed to write this data to a file
# and if all goes well I can make it plot the data at the end

import serial
import serial.tools.list_ports
import time
import sys
import glob
import string

#change as preferred
text_file_name = "receiver_data"



# this beautiful piece of code gets the active port, i.e. Arduino port
ports =  serial.tools.list_ports.comports()
for port in ports:
    port = str(port)
    port = port.split(' - ')
    serial_port = port[0]
    print(serial_port)

ser = serial.Serial(serial_port, baudrate = 115200)
start_time = time.time() # time in milliseconds

satellite_list = []
latitude_list = []
longitude_list = []
altitude_list = []
temperature_list = []
altitude_list = []

# this part of the code creates a new text file

class DelLetters:
    def __init__(self, keep=string.digits):
        self.comp = dict((ord(c), c) for c in keep)
    def __getitem__(self, k):
        return self.comp.get(k)
DD = DelLetters()

numbers = []
file = None
text_files = glob.glob("*.txt")

if text_files == []:
        print("allo")
        text_file_name = text_file_name + '.txt'
        file = open(text_file_name, "w+")
else:
    for text_file in text_files:
        number = text_file.translate(DD)
        if number != '':
            numbers.append(int(number))
        else:
            numbers.append(0)

if file == None:
    text_file_number = max(numbers)
    text_file_name = str(text_file_number + 1) + text_file_name +  '.txt'
    file = open(text_file_name, "w+")


print("writing to file: ", text_file_name)

while True:
    try:
        data = ser.readline()[:-2].decode('utf-8')
        data_list = data.split(',')
        
        if 'ovf' not in data_list:
            file.write(data + "\n")
            if len(data_list) == 8:
                print(data_list)
            elif len(data_list) == 6:
                print(data_list)
            
        millis = time.time() - start_time
    except:
        error = sys.exc_info()[0]
        print(error)
        ser.close()
        file.close()
        break
        
