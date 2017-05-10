#ifndef MOTION_H
#define MOTION_H
#include <cmath>
#include <complex>
#include <vector>

#include "gfft.h"

namespace motion {
	
	std::vector<std::vector<std::complex<double> > > generateSobel(unsigned log2_N, bool xy, bool shift);
	std::vector<std::vector<std::complex<double> > > edge(std::vector<std::vector<double> > in, std::vector<std::vector<std::complex<double> > > sobel, unsigned log2_N, bool flip);
	std::vector<std::vector<std::complex<double> > > convolve(std::vector<std::vector<std::complex<double> > > A, std::vector<std::vector<std::complex<double> > > B, unsigned log2_N);
}
	
#endif