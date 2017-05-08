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

#include <string.h>
#include <stdio.h>
#include <time.h>
#include <unistd.h>
#include <cmath>

#include "gfft.h"
#include "main.h"

unsigned Microseconds(void) {
    struct timespec ts;
    clock_gettime(CLOCK_REALTIME, &ts);
    return ts.tv_sec*1000000 + ts.tv_nsec/1000;
}

double* readBMP(char* filename)
{
    int i;
    FILE* f = fopen(filename, "rb");
    unsigned char info[54];
    fread(info, sizeof(unsigned char), 54, f); // read the 54-byte header

    // extract image height and width from header
    int width = *(int*)&info[18];
    int height = *(int*)&info[22];

    int size = 3 * width * height;
    unsigned char* data = new unsigned char[size]; // allocate 3 bytes per pixel
	double* fdata = new double[size];
    fread(data, sizeof(unsigned char), size, f); // read the rest of the data at once
    fclose(f);

	for(i=0;i<size;i++) {
		fdata[i] = (double)data[i]/255;
	}
	
    return fdata;
}

int main(int argc, char *argv[]) {
    
	BITMAPFILEHEADER bfh;
    BITMAPINFOHEADER bih;
	
	unsigned N = 512;

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
	
	
    gfft fft(GPU_FFT_FWD, 9);
	fft.clear();
	
	int x, y;
	
	for (y=250; y<=266; y++) {
        for (x=250; x<=266; x++) {
			fft.input(x,y,1);
		}
    }
	
	fft.execute();

    // Write output to bmp file
    for (y=0; y<N; y++) {
        for (x=0; x<N; x++) {
            fputc(std::sqrt(std::pow(fft.output(x,y).re, 2) + std::pow(fft.output(x,y).im, 2)), fp); // blue
            fputc(std::sqrt(std::pow(fft.output(x,y).re, 2) + std::pow(fft.output(x,y).im, 2)), fp); // green
            fputc(std::sqrt(std::pow(fft.output(x,y).re, 2) + std::pow(fft.output(x,y).im, 2)), fp); // red
        }
    }

    return 0;
}