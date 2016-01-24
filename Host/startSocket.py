#!/usr/bin/python
# whereIsArduino.py
import serial
import time
for com in range(0,4):
  try:
    PORT = '/dev/ttyUSB'+str(com)
    BAUD = 9600
    board = serial.Serial(PORT,BAUD)
    print 'Arduino erkannt an /dev/ttyACM'+str(com)
    print 'Hardware: %s' % board.__str__()
    #Arduino resets and reboots every time the serial command line is opened
    time.sleep(5)
    board.write(b"abc0")
    time.sleep( 1 )
    board.write(b"abc3")
    board.close()
		
    break
  except:
    print 'Kein Arduino an /dev/ttyUSB'+str(com)


