#!/usr/bin/env python

#import the required module. 
import RPi.GPIO as GPIO 
import time as t

# Set the mode of numbering the pins. 
GPIO.setmode(GPIO.BOARD) 

# GPIO pin 10 is the output. 
GPIO.setup(31, GPIO.OUT) 
GPIO.setup(33, GPIO.OUT) 
GPIO.setup(35, GPIO.OUT) 

gled = GPIO.PWM(31,50)
rled = GPIO.PWM(33,50)
bled = GPIO.PWM(35,50)

gled.start(50)
rled.start(50)
bled.start(50)

def output_rgb(r,g,b):
    gled.ChangeDutyCycle(g)
    rled.ChangeDutyCycle(r)
    bled.ChangeDutyCycle(b)
    

r = 0
g = 0
b = 0 
x = 0.02;
while 1: 
    for r in range(99):
	output_rgb(r,g,b)
	t.sleep(x)
    for g in range(99):
	output_rgb(r,g,b)
	t.sleep(x)
    for b in range(99):
	output_rgb(r,g,b)
	t.sleep(x)
    for r in range(99,0):
	output_rgb(r,g,b)
	t.sleep(x)
    for g in range(99,0):
	output_rgb(r,g,b)
	t.sleep(x)
    for b in range(99,0):
	output_rgb(r,g,b)
	t.sleep(x)
	

rled.stop()
GPIO.cleanup()

