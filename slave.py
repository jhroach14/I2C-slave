#!/usr/bin/env python
import pigpio
import time

#address slave responds to
I2C_ADDR=0x13

#method that gets called when i2c command recieved
def i2c(id, tick):
	global pi
	
	status, byteNum, data = pi.bsc_i2c(I2C_ADDR) #just addr as param = read
	
	if byteNum:
		print(str(byteNum) + "Byte command recieved\n");
		print("Data:\n"+data);
		
		status, byteNum, data = pi.bsc_i2c(I2C_ADDR, "hello world") #addr + string = write
	

pi = pigpio.pi()

if not pi.connected:
	exit();

event = pi.event_callback(pigpio.EVENT_BSC, i2c) #set i2c method above as callback
pi.bsc_i2c(I2C_ADDR) #Config BSC as i2c slave

time.sleep(600)
event.cancel()

pi.bsc_i2c(0)
pi.stop()

