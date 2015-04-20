
# outputfilename = 'arduino_bfield_data.csv'

from time import time,sleep
import serial
from sys import argv,exit

timeToTakeData = int(argv[1]) #seconds

# start listening to Arduino
ser = serial.Serial('/dev/ttyACM0', 9600) 

#when did we start?
begintime = time()

ser.flush()
#with open(outputfilename, 'w') as outfile:

while (time() - begintime) < timeToTakeData:
	
	 theline = ser.readline() #read a line from the Arduino
	 
	 print theline
	 print str(time())
	 
	 sleep(1)
	 
	 # outfile.write(theline) # write this to a file...
		 
     
     
     
