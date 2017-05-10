MOTION_PATH=gpu_fft/
MOTION_FLAGS=-lrt -lm -ldl
MOTION_O=mailbox.o gpu_fft_shaders.o gpu_fft_twiddles.o gpu_fft_trans.o gpu_fft_base.o gpu_fft.o gfft.o motion.o

GPU_FFT_PATH=gpu_fft/gpu_fft/

SHADERS=$(GPU_FFT_PATH)hex/shader_256.hex \
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

default: main.o $(MOTION_O)
	$(CXX) $(LDFLAGS) $(MOTION_FLAGS) main.o $(MOTION_O) -o testStill $(LDLIBS)

neopixelring: rpihw.o pwm.o ws2811.o mailbox.o dma.o neopixelring.o
	$(CC) -shared $(CLDFLAGS) -o lib/libneopixelring.so neopixelring.o rpihw.o pwm.o ws2811.o mailbox.o dma.o

motion.o: $(MOTION_PATH)motion.cpp
	$(CXX) -c $(CPPFLAGS) $(MOTION_PATH)motion.cpp
	
gfft.o: $(MOTION_PATH)gfft.cpp
	$(CXX) -c $(CPPFLAGS) $(MOTION_FLAGS) $(MOTION_PATH)gfft.cpp
	
main.o: main.cpp
	$(CXX) -c $(CPPFLAGS) $<
	
gpu_fft.o: $(GPU_FFT_PATH)gpu_fft.c
	$(CXX) -c $(CFLAGS) $(MOTION_FLAGS) $<
	
gpu_fft_base.o: $(GPU_FFT_PATH)gpu_fft_base.c
	$(CXX) -c $(CFLAGS) $(MOTION_FLAGS) $<
	
gpu_fft_twiddles.o: $(GPU_FFT_PATH)gpu_fft_twiddles.c
	$(CXX) -c $(CFLAGS) $(MOTION_FLAGS) $<

gpu_fft_shaders.o: $(GPU_FFT_PATH)gpu_fft_shaders.c $(SHADERS)
	$(CXX) -c $(CFLAGS) $(MOTION_FLAGS) $(GPU_FFT_PATH)gpu_fft_shaders.c
	
gpu_fft_mailbox.o: $(GPU_FFT_PATH)mailbox.c
	$(CXX) -c $(CFLAGS) $(MOTION_FLAGS) $<
	
gpu_fft_trans.o: $(GPU_FFT_PATH)gpu_fft_trans.c
	$(CXX) -c $(CFLAGS) $(MOTION_FLAGS) $<
	
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
