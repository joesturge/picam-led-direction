MOTION_PATH=gpu_fft/
MOTION_FLAGS = -lrt -lm -ldl

GPU_FFT_PATH=gpu_fft/gpu_fft/

SHADERS = $(GPU_FFT_PATH)hex/shader_256.hex \
    $(GPU_FFT_PATH)hex/shader_512.hex \
    $(GPU_FFT_PATH)hex/shader_1k.hex \
    $(GPU_FFT_PATH)hex/shader_2k.hex \
    $(GPU_FFT_PATH)hex/shader_4k.hex \
    $(GPU_FFT_PATH)hex/shader_8k.hex \
    $(GPU_FFT_PATH)hex/shader_16k.hex \
    $(GPU_FFT_PATH)hex/shader_32k.hex \
    $(GPU_FFT_PATH)hex/shader_64k.hex \
    $(GPU_FFT_PATH)hex/shader_128k.hex \
    $(GPU_FFT_PATH)hex/shader_256k.hex \
    $(GPU_FFT_PATH)hex/shader_512k.hex \
    $(GPU_FFT_PATH)hex/shader_1024k.hex \
    $(GPU_FFT_PATH)hex/shader_2048k.hex \
    $(GPU_FFT_PATH)hex/shader_4096k.hex \
    $(GPU_FFT_PATH)hex/shader_trans.hex

CPPFLAGS=-g -fpermissive -Wall -std=c++14 -Irpi_ws281x -Ilib
CFLAGS=-g -Irpi_ws281x
LDFLAGS=-g -L/home/pi/bir/lib -Ilib -I/usr/local/include
CLDFLAGS=-g -Llib -Ilib
LDLIBS=-lwiringPi -lraspicam 
# -lneopixelring

teststill: main.o motion.o
	$(CXX) $(LDFLAGS) main.o -o testStill $(LDLIBS)

neopixelring: rpihw.o pwm.o ws2811.o mailbox.o dma.o neopixelring.o
	$(CC) -shared $(CLDFLAGS) -o lib/libneopixelring.so neopixelring.o rpihw.o pwm.o ws2811.o mailbox.o dma.o

motion.o: $(SHADERS) $(MOTION_PATH)gfft.cpp $(MOTION_PATH)motion.cpp $(GPU_FFT_PATH)gpu_fft_trans.c $(GPU_FFT_PATH)mailbox.c $(GPU_FFT_PATH)gpu_fft.c $(GPU_FFT_PATH)gpu_fft_base.c $(GPU_FFT_PATH)gpu_fft_twiddles.c $(GPU_FFT_PATH)gpu_fft_shaders.c
	$(CXX) -c $(CPPFLAGS) $(MOTION_FLAGS) $(MOTION_PATH)gfft.cpp $(MOTION_PATH)motion.cpp $(GPU_FFT_PATH)gpu_fft_trans.c $(GPU_FFT_PATH)mailbox.c $(GPU_FFT_PATH)gpu_fft.c $(GPU_FFT_PATH)gpu_fft_base.c $(GPU_FFT_PATH)gpu_fft_twiddles.c $(GPU_FFT_PATH)gpu_fft_shaders.c
	
main.o: main.cpp
	$(CXX) -c $(CPPFLAGS) $<

neopixelring.o: lib/neopixelring.c
	$(CC) -c $(CFLAGS) $<

rpihw.o: rpi_ws281x/rpihw.c
	$(CC) -c $(CFLAGS) $<

pwm.o: rpi_ws281x/pwm.c
	$(CC) -c $(CFLAGS) $<

ws2811.o: rpi_ws281x/ws2811.c
	$(CC) -c $(CFLAGS) $<

mailbox.o: rpi_ws281x/mailbox.c
	$(CC) -c $(CFLAGS) $<

dma.o: rpi_ws281x/dma.c
	$(CC) -c $(CFLAGS) $<
	
clean:
	rm *.o
