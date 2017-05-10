CPPFLAGS=-g -fpermissive -Wall -std=c++11 -Irpi_ws281x -Ilib
CFLAGS=-g -Irpi_ws281x
LDFLAGS=-g -L/home/pi/bir/lib -Ilib -I/usr/local/include
CLDFLAGS=-g -Llib -Ilib
LDLIBS=-lwiringPi -lraspicam -lneopixelring

SRCS=main.cpp
OBJS=$(subst .cpp,.o,$(SRCS))

CSRCS=rpihw.c pwm.c ws2811.c mailbox.c dma.c
COBJS=$(subst .c,.o,$(CSRCS))

teststill: main.o neopixelring
	$(CXX) $(LDFLAGS) main.o -o testStill $(LDLIBS)

neopixelring: rpihw.o pwm.o ws2811.o mailbox.o dma.o neopixelring.o
	$(CC) -shared $(CLDFLAGS) -o lib/libneopixelring.so neopixelring.o rpihw.o pwm.o ws2811.o mailbox.o dma.o

main.o: main.cpp
	$(CXX) -c $(CPPFLAGS) $<

neopixelring.o: lib/neopixelring.c lib/neopixelring.h
	$(CC) -c $(CFLAGS) $<

rpihw.o: rpi_ws281x/rpihw.c rpi_ws281x/rpihw.h
	$(CC) -c $(CFLAGS) $<

pwm.o: rpi_ws281x/pwm.c rpi_ws281x/pwm.h
	$(CC) -c $(CFLAGS) $<

ws2811.o: rpi_ws281x/ws2811.c rpi_ws281x/ws2811.h
	$(CC) -c $(CFLAGS) $<

mailbox.o: rpi_ws281x/mailbox.c rpi_ws281x/mailbox.h
	$(CC) -c $(CFLAGS) $<

dma.o: rpi_ws281x/dma.c rpi_ws281x/dma.h
	$(CC) -c $(CFLAGS) $<

clean:
	rm *.o
