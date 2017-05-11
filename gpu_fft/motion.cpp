#include "motion.h"

namespace motion {

	std::vector<std::vector<std::complex<double> > > generateSobel(unsigned log2_N, bool xy=true, bool shift=false)
	{
		unsigned N = 1<<log2_N;

		gfft sobel(GPU_FFT_FWD, log2_N);
		sobel.clear();

		//double filter[][9] = {{ -1, 0, 1, -2, 0, 2, -1, 0, 1 } , { 1,  2,  1, 0,  0,  0, -1, -2, -1 }};

		double filter2d[25] = {-1,-2,0,2,1,-2,-4,0,4,2,0,0,0,0,0,2,4,0,-4,-2,1,2,0,-2,-1};
		//double filterL[9] = {-1,-1,-1,-1,8,-1,-1,-1,-1};

		unsigned x, y, i=0;

		for(y=0; y<5; y++) {
			for(x=0; x<5; x++) {
				//sobel.input(x, y, filter[xy ? 1:0][i]);
				sobel.input(x, y, filter2d[i]);
				i++;
			}
		}

		sobel.execute();

		std::vector<std::vector<std::complex<double> > > out;
		for(y=0; y<N; y++) {
			out.push_back(std::vector<std::complex<double> >());

			for(x=0; x<N; x++) {
				out.at(y).push_back(sobel.output(x, y, shift));
			}
		}

		return out;
	}

	std::vector<std::vector<std::complex<double> > > edge(std::vector<std::vector<double> > in, std::vector<std::vector<std::complex<double> > > sobel, unsigned log2_N, bool flip=false)
	{
		unsigned N = 1<<log2_N;

		gfft fft(GPU_FFT_FWD, log2_N);
		fft.clear();

		unsigned x, y;

		if(flip) {
			for(y=0; y<N; y++) {
				for(x=0; x<N; x++) {
					fft.input(x, y, in.at(N-x-1).at(N-y-1));
				}
			}
		}
		else {
			for(y=0; y<N; y++) {
				for(x=0; x<N; x++) {
					fft.input(x, y, in.at(x).at(y));
				}
			}
		}

		fft.execute();

		std::vector<std::vector<std::complex<double> > > out;

		for (y=0; y<N; y++) {
			out.push_back(std::vector<std::complex<double> > ());
			for (x=0; x<N; x++) {
				out.at(y).push_back(fft.output(x, y, false)*sobel.at(y).at(x));
			}
		}

		return out;
	}

	std::vector<std::vector<std::complex<double> > > convolve(std::vector<std::vector<std::complex<double> > > A, std::vector<std::vector<std::complex<double> > > B, unsigned log2_N)
	{
		unsigned N = 1<<log2_N, x, y;

		gfft ifft(GPU_FFT_REV, log2_N);

		for(y=0; y<N; y++) {
			for(x=0; x<N; x++) {
				ifft.input(x, y, A.at(x).at(y)*B.at(x).at(y));
			}
		}

		ifft.execute();

		std::vector<std::vector<std::complex<double> > > out;

		for (y=0; y<N; y++) {
			out.push_back(std::vector<std::complex<double> > ());
			for (x=0; x<N; x++) {
				out.at(y).push_back(ifft.output(x, y, true));

			}
		}

		return out;
	}
}
