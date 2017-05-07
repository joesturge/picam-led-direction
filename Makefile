CPPFLAGS=-g
LDFLAGS=-g
LDLIBS=
RASPICAM_OPTS=-I/usr/local/include -lraspicam

testgpio:
	g++ -fpermissive -Wall -IGPIOClass/ GPIOClass/GPIOClass.cpp GPIOClassTest.cpp -o testGPIO

teststill:
	g++ -fpermissive -Wall -IGPIOClass GPIOClass/GPIOClass.cpp flashy.cpp $(RASPICAM_OPTS) -o testStill

clean:
	rm *.bin
