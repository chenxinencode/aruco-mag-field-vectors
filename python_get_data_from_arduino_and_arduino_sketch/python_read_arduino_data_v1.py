timeToTakeData = 5 # seconds
outputfilename = 'arduino_bfield_data.csv'


# start listening to Arduino
import serial
ser = serial.Serial('/dev/ttyACM0', 9600) 

#when did we start?
from time import time,sleep
begintime = time()

ser.flush()
with open(outputfilename, 'w') as infile:

	while (time() - begintime) < timeToTakeData:
		
		 theline = ser.readline() #read a line from the Arduino
		 
		 print theline
		 
		 infile.write(theline) # write this to a file...
		 
     
     
     
