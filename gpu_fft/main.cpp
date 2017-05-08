/*
BCM2835 "GPU_FFT" release 2.0
Copyright (c) 2014, Andrew Holme.
All rights reserved.

Redistribution and use in source and binary forms, with or without
modification, are permitted provided that the following conditions are met:
    * Redistributions of source code must retain the above copyright
      notice, this list of conditions and the following disclaimer.
    * Redistributions in binary form must reproduce the above copyright
      notice, this list of conditions and the following disclaimer in the
      documentation and/or other materials provided with the distribution.
    * Neither the name of the copyright holder nor the
      names of its contributors may be used to endorse or promote products
      derived from this software without specific prior written permission.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY
DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
(INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
(INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/

#include <cstring>
#include <unistd.h>
#include <cmath>
#include <complex>
#include <vector>

#include "gfft.h"
#include "main.h"

std::vector<std::vector<std::complex<double> > > generateSobel(unsigned log2_N, bool xy=true, bool shift=false)
{
	unsigned N = 1<<log2_N;
	
	gfft sobel(GPU_FFT_FWD, log2_N);
	sobel.clear();
	
	double filter[][9] = {{ -1, 0, 1, -2, 0, 2, -1, 0, 1 } , { 1,  2,  1, 0,  0,  0, -1, -2, -1 }};
	
	unsigned x, y, i=0;
	
	for(y=0; y<3; y++) {
		for(x=0; x<3; x++) {
			sobel.input(x, y, filter[xy ? 1:0][i]);
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

std::vector<double> readBMP(std::string filename)
{
    int i;
    FILE* f = fopen(filename.c_str(), "rb");
    unsigned char info[54];
    fread(info, sizeof(unsigned char), 54, f); // read the 54-byte header

    // extract image height and width from header
    int width = *(int*)&info[18];
    int height = *(int*)&info[22];

    int size = 3 * width * height;
    unsigned char* data = new unsigned char[size]; // allocate 3 bytes per pixel
	
    fread(data, sizeof(unsigned char), size, f); // read the rest of the data at once
    fclose(f);
	
	std::vector<double> fdata;
	
	for(i=0;i<size;i++) {
		fdata.push_back((double)data[i]/255);
	}
	
    return fdata;
}

int main(int argc, char *argv[])
{
    
	BITMAPFILEHEADER bfh;
    BITMAPINFOHEADER bih;
	
	unsigned log2_N = 9;
	unsigned N = 1<<log2_N;

    // Create Windows bitmap file
    FILE *fp = fopen("output.bmp", "wb");
    if (!fp) return -666;

    // Write bitmap header
    memset(&bfh, 0, sizeof(bfh));
    bfh.bfType = 0x4D42; //"BM"
    bfh.bfSize = 512*512*3;
    bfh.bfOffBits = sizeof(bfh) + sizeof(bih);
    fwrite(&bfh, sizeof(bfh), 1, fp);

    // Write bitmap info
    memset(&bih, 0, sizeof(bih));
    bih.biSize = sizeof(bih);
    bih.biWidth = N;
    bih.biHeight = N;
    bih.biPlanes = 1;
    bih.biBitCount = 24;
    bih.biCompression = BI_RGB;
    fwrite(&bih, sizeof(bih), 1, fp);
	
	int x, y;
	/*
	//std::vector<double> lena = readBMP("lena.bmp");
	
    gfft fft(GPU_FFT_FWD, log2_N);
	fft.clear();
	
	//for(y=0; y<N; y++) {
	//	for(x=0; x<N; x++) {
	//		fft.input(x, y, lena.at(y*N + x);
	//	}
	//}
	
	for (y=250; y<=266; y++) {
        for (x=250; x<=266; x++) {
			fft.input(x,y,1);
		}
    }
	
	fft.execute();

    // Write output to bmp file
    for (y=0; y<N; y++) {
        for (x=0; x<N; x++) {
            fputc(std::abs(fft.outputShift(x,y))/2, fp); // blue
            fputc(std::abs(fft.outputShift(x,y))/2, fp); // green
            fputc(std::abs(fft.outputShift(x,y))/2, fp); // red
        }
    }
	*/
	
	std::vector<std::vector<std::complex<double> > > sobel = generateSobel(log2_N);
	
	char pixel;
	
	for (y=0; y<N; y++) {
        for (x=0; x<N; x++) {
			pixel = std::abs(sobel.at(y).at(x)) * 30;
            fputc(pixel, fp); // blue
            fputc(pixel, fp); // green
            fputc(pixel, fp); // red
        }
    }

    return 0;
}