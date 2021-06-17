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
start_time = time.time() # time in seconds

satellite_list = []
latitude_list = []
longitude_list = []
altitude_list = []
temperature_list = []
altitude_list = []

# this part of the code creates a new text file
text_files = glob.glob("*.txt")
class DelLetters:
    def __init__(self, keep=string.digits):
        self.comp = dict((ord(c), c) for c in keep)
    def __getitem__(self, k):
        return self.comp.get(k)
DD = DelLetters()

try:
    last_text_file = text_files[-1]
    text_file_number = last_text_file.translate(DD)
    if text_file_number == '':
        text_file_number = 0
    text_file_number = int(text_file_number)
    text_file_name = text_file_name + str(text_file_number + 1) + '.txt'
    file = open(text_file_name, "w+")
    file.write("Hallo")
    file.close()
except: 
    file = open(text_file_name + '.txt', "w+")
    file.write("Hallo")


while True:
    try:
        data = ser.readline()[:-2].decode('utf-8')
        data_list = data.split(',')
        if len(data_list) == 8:
            print(data)
        elif len(data_list) == 6:
            data_list.pop(0)
            print(data)
            
        
        millis = time.time() - start_time
    except:
        error = sys.exc_info()[0]
        print(error)
        ser.close()
        break
        
