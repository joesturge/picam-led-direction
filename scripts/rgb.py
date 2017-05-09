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

x = 2
y = 2

GPIO.output(31, 1)
t.sleep(x)
GPIO.output(31, 0)
t.sleep(y)
GPIO.output(33, 1)
t.sleep(x)
GPIO.output(33, 0)
t.sleep(y)
GPIO.output(35, 1)
t.sleep(x)
GPIO.output(35, 0)
t.sleep(y)


GPIO.output(31, 0)
GPIO.output(33, 0)
GPIO.output(35, 0)

GPIO.cleanup()
