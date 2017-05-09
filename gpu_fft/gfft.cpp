#include "gfft.h"

gfft::gfft(int direction, unsigned log2_N)
{
	this->fft_pass0 = new struct GPU_FFT;
	this->fft_pass0 = new struct GPU_FFT;
	this->trans = new struct GPU_FFT_TRANS;
	this->mailbox = mbox_open();
	
	this->N = 1<<log2_N;
	try{
		int ret;
		// Prepare 1st FFT pass
		ret = gpu_fft_prepare(this->mailbox, log2_N, direction, this->N, &this->fft_pass0);
		if (ret) {
			throw(1);
		}
		// Prepare 2nd FFT pass
		ret = gpu_fft_prepare(this->mailbox, log2_N, direction, this->N, &this->fft_pass1);
		if (ret) {
			gpu_fft_release(this->fft_pass0);
			throw(2);
		}
		// Transpose from 1st pass output to 2nd pass input
		ret = gpu_fft_trans_prepare(mailbox, fft_pass0, fft_pass1, &this->trans);
		if (ret) {
			gpu_fft_release(this->fft_pass0);
			gpu_fft_release(this->fft_pass1);
			throw(3);
		}
	}
	catch(int e) {
		std::cout << "Failed to Create GFFT class!" << e << std::endl;
	}
}

gfft::~gfft()
{
    gpu_fft_release(this->fft_pass0);
    gpu_fft_release(this->fft_pass1);
    gpu_fft_trans_release(this->trans);
	mbox_close(this->mailbox);
}

void gfft::input(unsigned x, unsigned y, double value)
{
	GPU_FFT_ROW(fft_pass0, in, y)[x].re = value;
}

void gfft::input(unsigned x, unsigned y, std::complex<double> value)
{
	GPU_FFT_ROW(fft_pass0, in, y)[x] = toGComplex(value);
}

void gfft::execute()
{
    gpu_fft_execute(this->fft_pass0);
    gpu_fft_trans_execute(this->trans);
    gpu_fft_execute(this->fft_pass1);
}

std::complex<double> gfft::output(unsigned x, unsigned y, bool shift=false)
{
	if(shift) {
			return toCComplex(GPU_FFT_ROW(this->fft_pass1, out, (y+this->N/2)%N)[(x+this->N/2)%N]);
	}
	return toCComplex(GPU_FFT_ROW(this->fft_pass1, in, y)[x]);
}

void gfft::clear()
{
	int x, y;
	struct GPU_FFT_COMPLEX *row;
	// Clear input array
    for (y=0; y<N; y++) {
        row = GPU_FFT_ROW(this->fft_pass0, in, y);
        for (x=0; x<N; x++) row[x].re = row[x].im = 0;
    }
}

std::complex<double> toCComplex(GPU_FFT_COMPLEX gpuComplex)
{
	return std::complex<double>(gpuComplex.re, gpuComplex.im);
}

GPU_FFT_COMPLEX toGComplex(std::complex<double> cComplex)
{
	GPU_FFT_COMPLEX c;
	c.re = cComplex.real();
	c.im = cComplex.imag();
	return c;
}
