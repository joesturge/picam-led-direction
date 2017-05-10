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

#include "motion.h"
#include "main.h"

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
	
	unsigned x, y;
	
	std::vector<double> data = readBMP("picam.bmp");
	std::vector<std::vector<double> > imageR;
	for(y=0;y<N;y++) {
		imageR.push_back(std::vector<double> ());
		for(x=0;x<N;x++) {
			imageR.at(y).push_back(data.at(3*(y*N+x)));
		}	
	}
	
	std::vector<std::vector<double> > imageB;
	for(y=0;y<N;y++) {
		imageB.push_back(std::vector<double> ());
		for(x=0;x<N;x++) {
			imageB.at(y).push_back(data.at(3*(y*N+x)+2));
		}	
	}
		
	std::vector<std::vector<std::complex<double> > > filter = motion::generateSobel(log2_N, true, false);
	
	std::vector<std::vector<std::complex<double> > > edgeA = motion::edge(imageR, filter, log2_N, true);
	std::vector<std::vector<std::complex<double> > > edgeB = motion::edge(imageB, filter, log2_N, false);
	std::vector<std::vector<std::complex<double> > > convolution = motion::convolve(edgeA, edgeB, log2_N);
	
	double temp;
	unsigned char pixel;
	
	for (y=0; y<N; y++) {
        for (x=0; x<N; x++) {
			
			temp = std::abs(convolution.at(y).at(x))/std::pow(10, 6);
			pixel = temp>255?(unsigned char)255:(unsigned char)temp;
            fputc(pixel, fp); // blue
            fputc(pixel, fp); // green
            fputc(pixel, fp); // red
        }
    }
	
    return 0;
}