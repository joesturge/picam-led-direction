/*
 * PiCAM Main Project file
 */
#include <chrono> //sleeping
#include <thread> //sleeping
#include <fstream>
#include <iostream>
#include <unistd.h>
#include <string>
#include <signal.h>
#include <raspicam/raspicam.h>
#include <wiringPi.h>
#include <sys/stat.h>

#include <vector>
#include <complex>
#include "gpu_fft/motion.h"
#include "gpu_fft/gfft.h"

#include "neopixelring.h"

using namespace std;

#define RED_LED 6
#define BLU_LED 19
#define GRN_LED 13
#define M_PI    3.14159265358979323846

static void ctrl_c_handler(int signum)
{
		neopixelClose();
		//Camera->release();
		cout << "CTL-C Handled." << endl;
		exit(1);
}

static void setup_handlers(void)
{
    struct sigaction sa;
		sa.sa_handler = ctrl_c_handler;

    sigaction(SIGINT, &sa, NULL);
    sigaction(SIGTERM, &sa, NULL);
}

void millisleep(uint t){
	std::this_thread::sleep_for(std::chrono::milliseconds(t));
}

void microsleep(uint t){
	std::this_thread::sleep_for(std::chrono::microseconds(t));
}

void gpioInit() {
	wiringPiSetupGpio();

	pinMode(RED_LED, OUTPUT);
	pinMode(BLU_LED, OUTPUT);
	pinMode(GRN_LED, OUTPUT);
}

void ledPulse(int ledpin, uint onTime=3850) {
	digitalWrite(ledpin, HIGH);
	microsleep(onTime);
	digitalWrite(ledpin, LOW);
}

void led2Pulse(int led1pin, int led2pin, uint onTime=3850) {
	digitalWrite(led1pin, HIGH);
	digitalWrite(led2pin, HIGH);
	microsleep(onTime);
	digitalWrite(led1pin, LOW);
	digitalWrite(led2pin, LOW);
}

void flashGrab(raspicam::RaspiCam* cam, int led) {
	cout<<"Capturing image"<<endl;

	if (!cam->grab_dontwait()) {
		cerr<<"Error grabbing frame"<<endl;
	}
	ledPulse(led);
}

uint looptime;
uint count;

void ledPulseTime(uint onTime, uint offTime, uint totalTime, int led1, int led2, int led3) {
	looptime = ((onTime+offTime)*2/1000);
  count = (totalTime/looptime)+1;
	//cout<<"Doing "<<count<<" loops @ "<<looptime<<"ms each"<<endl;

	for (uint x=0;x<=count;x++)
	{
		ledPulse(led1,onTime);
		microsleep(offTime);
		//led2Pulse(led2,led3,onTime);
		ledPulse(led3,onTime);
		microsleep(offTime);
		//ledPulse(led3,onTime);
		//microsleep(offTime);
	}
}

void flashRGBGrab(raspicam::RaspiCam* cam, int led1, int led2, int led3, uint onTime, uint offTime) {
	//cout<<"Capturing RGB image"<<endl;

	ledPulseTime(onTime,offTime,20,led1,led2,led3);
	if (!cam->grab_dontwait()) {
		cerr<<"Error grabbing frame"<<endl;
	}
	//msleep(5);
	ledPulseTime(onTime,offTime,100,led1,led2,led3);

	//msleep(50);
}

void writeOut(raspicam::RaspiCam* cam, std::string filename, uint filesize) {
	unsigned char *data = new unsigned char[cam->getImageTypeSize( raspicam::RASPICAM_FORMAT_RGB )];
	cam->retrieve( data );
	std::ofstream outFile ( filename.c_str(), std::ios::binary );

	outFile<<"P6\n"<<cam->getWidth() <<" "<<cam->getHeight() <<" 255\n";
	outFile.write( (char*)data, filesize );
	//cout<<"Image saved as "<<filename<<endl;
	delete data;
}

/*void testCB(void* x) {
	cout<<"Test Callback"<<endl;
	//return 0;
}*/

void camInit(raspicam::RaspiCam* Camera) {
	cout<<"Initing cam"<<endl;

	// override camera settings here
	Camera->setWidth(640);
	Camera->setHeight(480);
	//Camera->setShutterSpeed(5000);
	//Camera->setExposure(raspicam::RASPICAM_EXPOSURE_VERYLONG);
	//uint tdata = 1;
	//Camera->setUserCallback(&testCB,(void*)&tdata);

	if ( !Camera->open()) {
		cerr<<"Error opening camera"<<endl;
		exit(1);
	}

	millisleep(2000); //2s
	Camera->grab();
	millisleep(100); // 0.1s
	Camera->grab();
	millisleep(100); // 0.1s

	struct timespec start, finish;
	double elapsed;
	clock_gettime(CLOCK_MONOTONIC, &start);
	Camera->grab();
	clock_gettime(CLOCK_MONOTONIC, &finish);
	elapsed = (finish.tv_sec - start.tv_sec);
  elapsed += (finish.tv_nsec - start.tv_nsec) / 1000000000.0;
	cout<<"grab timing: "<<elapsed<<endl;

}

std::vector<std::vector<std::vector<double> > > toSquareImage(unsigned char *input, unsigned width, unsigned height, int log2_N) {
	unsigned N = 1<<log2_N;

	if(N>height || N>width) {
		cerr << "Cannot crop image for convolution. 2^(Log2(N)) must be less than image witdth and height!" << endl;
		//return;
	}

	unsigned bufferUD = (height-N)/2;
	unsigned bufferLR = (width-N)/2;

	unsigned startIndex = (width*bufferUD+bufferLR)*3;

	std::vector<std::vector<std::vector<double> > > out;
	out.push_back(std::vector<std::vector<double> > ());
	out.push_back(std::vector<std::vector<double> > ());
	out.push_back(std::vector<std::vector<double> > ());

	unsigned x, y, i=startIndex;
	for(y=0;y<N;y++) {
		out.at(0).push_back(std::vector<double> ());
		out.at(1).push_back(std::vector<double> ());
		out.at(2).push_back(std::vector<double> ());
		for(x=0; x<N; x++) {
			out.at(0).at(y).push_back((double)input[i]/255);
			out.at(1).at(y).push_back((double)input[i+1]/255);
			out.at(2).at(y).push_back((double)input[i+2]/255);
			i+= 3;
		}
		i += bufferLR*6; // get from right edge of crop to next left edge of crop
	}

	return out;

}

void writeOutCrop(raspicam::RaspiCam* cam, std::string filename, uint filesize) {
	unsigned log2_N = 8;
	unsigned N = 1<<log2_N;

	unsigned char *data = new unsigned char[cam->getImageTypeSize( raspicam::RASPICAM_FORMAT_RGB )];
	cam->retrieve( data );
	std::ofstream outFile ( filename.c_str(), std::ios::binary );

	std::vector<std::vector<std::vector<double> > > image = toSquareImage(data, 640, 480, 8);

	outFile<<"P6\n"<< N <<" "<< N <<" 255\n";
	//outFile.write( (char*)data, filesize );

	unsigned x, y;
	for(y=0; y<N; y++) {
		for(x=0; x<N; x++) {
			outFile.put((unsigned)(image.at(0).at(y).at(x)*255));
			outFile.put((unsigned)(image.at(1).at(y).at(x)*255));
			outFile.put((unsigned)(image.at(2).at(y).at(x)*255));
		}
	}

	//cout<<"Image saved as "<<filename<<endl;
	delete data;
}

void writeOutConvolute(raspicam::RaspiCam* cam, std::string filename, uint filesize, std::vector<std::vector<std::complex<double> > > filter) {
	unsigned log2_N = 8;
	unsigned N = 1<<log2_N;

	unsigned char *data = new unsigned char[cam->getImageTypeSize( raspicam::RASPICAM_FORMAT_RGB )];
	cam->retrieve( data );
	std::ofstream outFile ( filename.c_str(), std::ios::binary );

	std::vector<std::vector<std::vector<double> > > image = toSquareImage(data, 640, 480, log2_N);

	std::vector<std::vector<std::complex<double> > > edgeA = motion::edge(image.at(0), filter, log2_N, true);
	std::vector<std::vector<std::complex<double> > > edgeB = motion::edge(image.at(2), filter, log2_N, false);
	std::vector<std::vector<std::complex<double> > > convolution = motion::convolve(edgeA, edgeB, log2_N);

	outFile<<"P6\n"<< N <<" "<< N <<" 255\n";

	unsigned x, y;
	unsigned char pixel;
	double temp;

	for(y=0; y<N; y++) {
		for(x=0; x<N; x++) {
			temp = std::abs(convolution.at(y).at(x))/std::pow(10, 6);
			pixel = (unsigned char) temp;

			outFile.put(pixel);
			outFile.put(pixel);
			outFile.put(pixel);
		}
	}

	//cout<<"Image saved as "<<filename<<endl;
	delete data;
}

std::complex<double> getMotion(raspicam::RaspiCam* cam, std::vector<std::vector<std::complex<double> > > filter, std::string filename) {
	unsigned log2_N = 8;
	unsigned N = 1<<log2_N;

	unsigned char *data = new unsigned char[cam->getImageTypeSize( raspicam::RASPICAM_FORMAT_RGB )];
	cam->retrieve( data );

	std::ofstream outFile ( (filename+".ppm").c_str(), std::ios::binary );
	std::ofstream outFile2 ( (filename+"b.ppm").c_str(), std::ios::binary );

	outFile<<"P6\n"<< N <<" "<< N <<" 255\n";
	outFile2<<"P6\n"<< N <<" "<< N <<" 255\n";

	std::vector<std::vector<std::vector<double> > > image = toSquareImage(data, 640, 480, log2_N);

	std::vector<std::vector<std::complex<double> > > edgeA = motion::edge(image.at(0), filter, log2_N, true);
	std::vector<std::vector<std::complex<double> > > edgeB = motion::edge(image.at(2), filter, log2_N, false);
	std::vector<std::vector<std::complex<double> > > convolution = motion::convolve(edgeA, edgeB, log2_N);

	unsigned x, y, xmax=0, ymax=0;
	unsigned char pixel;
	double maxValue=0, temp;
    uint N_middle_low = (N/2)-10;
	uint N_middle_hi = (N/2)+10;
	
	double middleValue = (std::abs(convolution.at(N/2-1).at(N/2-1))+ \
	std::abs(convolution.at(N/2-1).at(N/2))+ \
	std::abs(convolution.at(N/2).at(N/2-1))+ \
	std::abs(convolution.at(N/2).at(N/2)))/4;
	
	bool isMoving = false;

	for(y=0; y<N; y++) {
		for(x=0; x<N; x++) {
			temp = std::abs(convolution.at(y).at(x));

			if((x>N_middle_low)&&(x<N_middle_hi)&&(y>N_middle_low)&&(y<N_middle_hi)) {
				temp = 0;
			}

			if(temp>maxValue && temp>middleValue/2) {
				maxValue = temp;
				xmax = x;
				ymax = y;
			}
		}
	}

	for(y=0; y<N; y++) {
		for(x=0; x<N; x++) {
			pixel = (unsigned char) ((std::abs(convolution.at(y).at(x))/maxValue)*127);

			if((x>N_middle_low)&&(x<N_middle_hi)&&(y>N_middle_low)&&(y<N_middle_hi)) {
				pixel = 0;
			}

			outFile.put(pixel);
			outFile.put(pixel);
			outFile.put(pixel);

			outFile2.put(image.at(0).at(y).at(x)*255);
			outFile2.put(image.at(1).at(y).at(x)*255);
			outFile2.put(image.at(2).at(y).at(x)*255);
		}
	}
	//cout<<"Image saved as "<<filename<<endl;
	delete data;
	
	if(maxValue == 0) {
		std::complex<double> out(0,0);
		return out;
	}
	std::complex<double> out(((double)xmax-(N/2)-0.5)/N, -((double)ymax-(N/2)-0.5)/N);
	return out;
}

uint prev_led_no = 0;
uint prev_opp_led = 0;

void complexToNeoPixel(std::complex<double> cmpl) {
	uint max_abs = 1; // Max magnitude
	float magnitude = std::abs(cmpl);
	uint color = (magnitude/max_abs)*255;
	float angle = std::arg(cmpl); // radians
  uint led_no = (((angle+M_PI)/(2*M_PI))*12)-3;
	if (led_no < 0) { led_no = led_no + 12; }
	//cout<<"raw_mag: "<<magnitude<<" angle: "<<angle<<endl;

	neopixelClear(prev_led_no);
	neopixelClear(prev_opp_led);
	uint opp_led = led_no + 6;
	if (opp_led > 11) { opp_led = opp_led - 12; }
    neopixelUpdate(led_no,color,(255-color),0);
    //neopixelUpdate(opp_led,color,(255-color),0);
	prev_led_no = led_no;
	prev_opp_led = opp_led;
}

void neopixelFlash3Red() {
	neopixelUpdate(0,200,0,0);
	millisleep(200);
	neopixelClear(0);
	millisleep(100);
	neopixelUpdate(0,200,0,0);
	millisleep(200);
	neopixelClear(0);
	millisleep(100);
	neopixelUpdate(0,200,0,0);
	millisleep(200);
	neopixelClear(0);
	millisleep(100);
}

void neopixelTest() {
	for(uint i=0; i<=255; i=i+20) {
		for(uint x=0; x<=5; x++) {
			neopixelUpdate(x,i,(255-i),0);
			neopixelUpdate(x+6,i,(255-i),0);
			millisleep(50);
		}
		for(uint x=0; x<=5; x++) {
			neopixelClear(x);
			neopixelClear(x+6);
			millisleep(50);
		}
	}
}

int main ( int argc, char **argv ) {

	setup_handlers();
	piHiPri(90); // set high priority
	gpioInit();

	float duty, freq;
	unsigned log2_N = 8;

  if (argc == 2) {
		duty = atof(argv[1]);
		freq = atof(argv[2]);
	} else if ((argc == 1)){
		std::cout << "Using default duty & freq" << '\n';
		duty = 0.1;
		freq = 15;
	}
	std::cout << "Duty: "<<duty<<" Freq: "<<freq<<std::endl;

	uint onTime = (1000000.0/freq)*duty;
	uint offTime = (1000000.0/freq)*(1.0-(2*duty))/2.0;
	cout<<"on/off: "<<onTime<<"/"<<offTime<<endl;

	neopixelInit();
	//neopixelTest();
	neopixelFlash3Red();

	raspicam::RaspiCam* Camera = new raspicam::RaspiCam;
	camInit(Camera);

	//uint filesize = Camera->getImageTypeSize( raspicam::RASPICAM_FORMAT_RGB );
	struct timespec start, finish, filets;
	double elapsed;
	clock_gettime(CLOCK_MONOTONIC, &filets);

    std::string base = "pics/set"+std::to_string(int(filets.tv_sec));
	mkdir(base.c_str(),0755);

	std::vector<std::vector<std::complex<double> > > filter = motion::generateSobel(log2_N, true, false);

	for(uint i=0; i<=9; i++) {

		clock_gettime(CLOCK_MONOTONIC, &start);
		flashRGBGrab( Camera, RED_LED, GRN_LED, BLU_LED, onTime, offTime);
		clock_gettime(CLOCK_MONOTONIC, &finish);
		elapsed = (finish.tv_sec - start.tv_sec);
		elapsed += (finish.tv_nsec - start.tv_nsec) / 1000000000.0;
		cout<<"flashRGBGrab timing: "<<elapsed<<endl;

		std::string fn = base+"/picam-rgb"+std::to_string(i);

		//writeOutConvolute( Camera, fn, filesize, filter );
		std::complex<double> mVector = getMotion( Camera, filter, fn );
		cout<<mVector<<endl;
		complexToNeoPixel(mVector);

		//writeOut( Camera, fn, filesize );

  }

	millisleep(1000);
	Camera->release();
	neopixelClose();
	cout << "Exiting." << endl;
	return 0;
}
