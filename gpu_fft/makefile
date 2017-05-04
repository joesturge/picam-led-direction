S = gpu_fft/hex/shader_256.hex \
    gpu_fft/hex/shader_512.hex \
    gpu_fft/hex/shader_1k.hex \
    gpu_fft/hex/shader_2k.hex \
    gpu_fft/hex/shader_4k.hex \
    gpu_fft/hex/shader_8k.hex \
    gpu_fft/hex/shader_16k.hex \
    gpu_fft/hex/shader_32k.hex \
    gpu_fft/hex/shader_64k.hex \
    gpu_fft/hex/shader_128k.hex \
    gpu_fft/hex/shader_256k.hex \
    gpu_fft/hex/shader_512k.hex \
    gpu_fft/hex/shader_1024k.hex \
    gpu_fft/hex/shader_2048k.hex \
    gpu_fft/hex/shader_4096k.hex \
    gpu_fft/hex/shader_trans.hex

C = gpu_fft/mailbox.c gpu_fft/gpu_fft.c gpu_fft/gpu_fft_base.c gpu_fft/gpu_fft_twiddles.c gpu_fft/gpu_fft_shaders.c

C2D = $(C) main.cpp gpu_fft/gpu_fft_trans.c

H2D = gpu_fft/gpu_fft.h gpu_fft/mailbox.h gpu_fft/gpu_fft_trans.h gpu_fft/hello_fft_2d_bitmap.h

F = -lrt -lm -ldl

default:   $(S) $(C1D) $(H1D)
	g++ -o main.bin $(F) $(C2D)

clean:
	rm -f *.bin
