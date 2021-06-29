

import time, os
from urllib.request import urlopen as uo

import os, glob
import pandas as pd
import numpy as np

pump_dir = 'pumps/levels/*.csv'

import os
from simple_pid import PID

time_window = 18 # minutes - levelsensor logs data every 5 seconds, using this a priori
def get_last_n_lines(file_name, N):
    # Create an empty list to keep the track of last N lines
    list_of_lines = []
    # Open file for reading in binary mode
    with open(file_name, 'rb') as read_obj:
        # Move the cursor to the end of the file
        read_obj.seek(0, os.SEEK_END)
        # Create a buffer to keep the last read line
        buffer = bytearray()
        # Get the current position of pointer i.e eof
        pointer_location = read_obj.tell()
        # Loop till pointer reaches the top of the file
        while pointer_location >= 0:
            # Move the file pointer to the location pointed by pointer_location
            read_obj.seek(pointer_location)
            # Shift pointer location by -1
            pointer_location = pointer_location -1
            # read that byte / character
            new_byte = read_obj.read(1)
            # If the read byte is new line character then it means one line is read
            if new_byte == b'\n':
                # Save the line in list of lines
                list_of_lines.append(buffer.decode()[::-1])
                # If the size of list reaches N, then return the reversed list
                if len(list_of_lines) == N:
                    return list(reversed(list_of_lines))
                # Reinitialize the byte array to save next line
                buffer = bytearray()
            else:
                # If last read character is not eol then add it in buffer
                buffer.extend(new_byte)
        # As file is read completely, if there is still data in buffer, then its first line.
        if len(buffer) > 0:
            list_of_lines.append(buffer.decode()[::-1])
    # return the reversed list
    return list(reversed(list_of_lines))

pump_dir = os.path.expanduser('~/Nextcloud/docs/projects/PhD/data/pumps/levels/*.csv')
most_recent = 0
for f in glob.iglob(pump_dir):
    last_updated = os.path.getmtime(f)
    if(last_updated > most_recent):
        most_recent = last_updated
        level_file = f
# print(level_file, most_recent)

# Gains determined by trial-and-error for 2.2 cm^2 cell with Celgard 4560 separator, 0.1 M V(acac)3 0.3 M TEABF4 in acetonitrile 
# If controller is driving pumps in wrong direction, change sign of all gains (or change sign of level sensor signal)
flowRate = 92 # duty cycle for 20 mL/min

pid = PID(Kp=-100, Ki=-0.005, Kd=0.0, setpoint=0, sample_time=None, 
    output_limits=(-(255-flowRate), 255-flowRate), auto_mode=True, proportional_on_measurement=False, error_map=None)
pid.set_auto_mode(False)
time.sleep(3)
pid.set_auto_mode(True, last_output=-.28)


updateTimer = time.perf_counter()
while True:
    if(time.perf_counter()-updateTimer > 5):

        try:
            last_readings = np.genfromtxt(get_last_n_lines(level_file,time_window*60/5),delimiter=',')
            last_readings = last_readings[~np.isnan(last_readings).any(axis=1), :]
            # print(last_readings)
            diff = last_readings[:,1]-last_readings[:,3]
            avg_diff = np.mean(diff)
            v = avg_diff # imbalance in mL
        except:
            pass

        # Compute new output from the PID according to the systems current value
        
        control = pid(v)
        print(v, control,pid.components)
        control = round(control)
        # Feed the PID output to the system and get its current value

        if (control > 0): 
            flowRateA = flowRate + control
            flowRateB = flowRate
            
        else:
            flowRateA = flowRate
            flowRateB = flowRate - control
        try:
            uo("http://localhost:8000/"+"write/"+'<a,{}>'.format(flowRateA))
            uo("http://localhost:8000/"+"write/"+'<b,{}>'.format(flowRateB))
        except:
            pass

        updateTimer = time.perf_counter()

