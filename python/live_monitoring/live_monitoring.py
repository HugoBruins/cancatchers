# this code opens the ground station port and reads from the Arduino
# after that it will proceed to write this data to a file
# and if all goes well I can make it plot the data at the end

import serial
import serial.tools.list_ports
import time
import sys
import os
import datetime
import matplotlib.pyplot as plt

#change as preferred
text_file_name = "receiver_data"
receiver_baudrate = 115200
plot_1_name = "temperature pressure altitude"

# this will plot all the data afterward, the text file is still made
# however, so replotting is always possible. 
def make_plot():
    plt.close('all')
    fig, ax = plt.subplots(3, figsize=(16,9))
    ax[0].plot(time_list, temperature_list, color = 'red')
    ax[0].set_xlabel('time (s)')
    ax[0].set_ylabel('temperature (℃)')
    ax[0].grid()
    ax[1].plot(time_list, pressure_list, color = 'orange')
    ax[1].set_xlabel('time (s)')
    ax[1].set_ylabel('pressure (Pa)')
    ax[1].grid()
    ax[2].plot(time_list, altitude_list, color = 'red')
    ax[2].set_xlabel('time (s)')
    ax[2].set_ylabel('altitude (m)')
    ax[2].grid()
    plt.tight_layout()
    fig.savefig(os.path.join(path, plot_1_name + '.png'), dpi = 185, pad_inches = 10)

# this part of the code creates a new text file
def create_file_name(wanted_folder_name, wanted_text_name):
    folder_name = wanted_folder_name + " " + str(datetime.datetime.now())[:-7]
    folder_name = folder_name.replace(":", ".")
    os.makedirs(folder_name)
    path = os.path.join(os.getcwd(), folder_name)
    file = open(os.path.join(path, wanted_text_name + '.txt'), "w+")
    return file, path

# this beautiful piece of code gets the active port, i.e. Arduino port
# if it's connected via usb obviously
ports =  serial.tools.list_ports.comports()
for port in ports:
    port = str(port)
    port = port.split(' - ')
    serial_port = port[0]
    print(serial_port)

ser = serial.Serial(serial_port, baudrate = 115200)
satellite_list,latitude_list,longitude_list = [], [], []
altitude_list, temperature_list, pressure_list, time_list = [], [], [], []

file, path = create_file_name("data", "receiver_data")
print("writing to file: ", file.name)

start_time = time.time()
while True:
    try:
        data = ser.readline()[:-2].decode('utf-8')
        data_list = data.split(',')
        
        #some filtering for garbage data, i.e. if the received struct is wrong
        if 'ovf' not in data_list:
            # if the gps is not working, some adjustments need to be made
            if len(data_list) == 6:
                data = data[2:]
                data = "99,0,0" + data
                print(data)
            file.write(data + "\n")
            data_list = data.split(',')
            if len(data_list) == 8:
                # if the amount of satellites is 99, the cansat is running
                # in fallback mode. 
                if data_list[0] != '99':
                    satellites = int(data_list[0])
                else:
                    satellite_list.append(0)
                
                latitude = float(data_list[1])
                longitude = float(data_list[2])
                altitude = float(data_list[3])
                temperature = float(data_list[4])
                pressure = float(data_list[5])
                current_time = time.time() - start_time
                
                latitude_list.append(latitude)
                longitude_list.append(longitude)
                altitude_list.append(altitude)
                temperature_list.append(temperature)
                pressure_list.append(pressure)
                time_list.append(current_time)                        
    # when somebody stops the code
    except:
        error = sys.exc_info()[0]
        print(error)
        print("The data was written to: ", text_file_name)
        ser.close()
        file.close()
        make_plot()
        break
