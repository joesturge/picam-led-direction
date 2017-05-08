#ifndef GFFT_H
#define GFFT_H

#include <string.h>
#include <stdio.h>
#include <time.h>
#include <unistd.h>
#include <cmath>
#include <iostream>

#include "gpu_fft/gpu_fft_trans.h"
#include "gpu_fft/mailbox.h"

#define GPU_FFT_ROW(fft, io, y) ((fft)->io+(fft)->step*(y))

class gfft
{
    public:
        gfft(int direction, unsigned log2_N);
        virtual ~gfft();
		
		void input(unsigned x, unsigned y, double value);
		void execute();
		GPU_FFT_COMPLEX output(unsigned x, unsigned y);
		GPU_FFT_COMPLEX outputShift(unsigned x, unsigned y);
		void clear();
    protected:
    private:
		int N;
		struct GPU_FFT_TRANS *trans;
		struct GPU_FFT *fft_pass0;
		struct GPU_FFT *fft_pass1;
		int mailbox;
};

#endif // GFFT_H