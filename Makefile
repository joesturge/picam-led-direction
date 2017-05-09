CPPFLAGS=-g -fpermissive -Wall -std=c++11
LDFLAGS=-g
LDLIBS=-I/usr/local/include -lwiringPi -lraspicam

SRCS=main.cpp
OBJS=$(subst .cpp,.o,$(SRCS))

teststill: main.o
	$(CXX) $(LDFLAGS) main.o -o testStill $(LDLIBS)

%.o: %.cpp
	$(CXX) -c $(CPPFLAGS) $<

clean:
	rm *.o testStill
