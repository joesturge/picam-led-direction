CPPFLAGS=-g -fpermissive -Wall -IGPIOClass/ GPIOClass/GPIOClass.cpp -std=c++11
LDFLAGS=-g
LDLIBS=-IGPIOClass/
RASPICAM_OPTS=-I/usr/local/include -lraspicam

SRCS=GPIOClass.cpp flashy.cpp
OBJS=$(subst .cpp,.o,$(SRCS))

testgpio: GPIOClassTest.o
	$(CXX) $(LDFLAGS) GPIOClassTest.o -o testGPIO $(LDLIBS)# -IGPIOClass/ GPIOClass/GPIOClass.cpp

teststill: flashy.o
	$(CXX) $(LDFLAGS) flashy.o $(RASPICAM_OPTS) -o testStill $(LDLIBS)# -IGPIOClass GPIOClass/GPIOClass.cpp

%.o: %.cpp
	$(CXX) -c $(CPPFLAGS) $<

clean:
	rm *.o testGPIO testStill
