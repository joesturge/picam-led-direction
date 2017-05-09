/*
 * PiCAM Main Project file
 */
//#include <ctime>
#include <chrono> //sleeping
#include <thread> //sleeping
#include <fstream>
#include <iostream>
#include <unistd.h>
#include <string>
#include <raspicam/raspicam.h>
//#include "GPIOClass.h"
#include <wiringPi.h>

using namespace std;

#define RED_LED 6
#define BLU_LED 13
#define GRN_LED 19

void msleep(uint t){
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
	// Make sure we pulse for at least 100ms
	looptime = ((onTime*2+offTime)/1000);
  count = (totalTime/looptime)+1;
	//cout<<"Doing "<<count<<" loops @ "<<looptime<<"ms each"<<endl;

	for (uint x=0;x<=count;x++)
	{
		ledPulse(led1,onTime);
		led2Pulse(led2,led3,onTime);
		microsleep(offTime);
		//ledPulse(led3,onTime);
		//microsleep(offTime);
	}
}

void flashRGBGrab(raspicam::RaspiCam* cam, int led1, int led2, int led3) {
	//cout<<"Capturing RGB image"<<endl;

	ledPulseTime(3850,8970,20,led1,led2,led3);
	if (!cam->grab_dontwait()) {
		cerr<<"Error grabbing frame"<<endl;
	}
	//msleep(5);
	ledPulseTime(3850,8970,100,led1,led2,led3);
	//ledPulse100(3000,6993,led1,led2,led3);
	//ledPulse100(60,139,led1,led2,led3);
	/*microsleep(10000);
	ledPulse(led1);
	microsleep(8970);
	ledPulse(led2);
	microsleep(8970);
	ledPulse(led3);*/
	//msleep(50);
}

void writeOut(raspicam::RaspiCam* cam, std::string filename, uint filesize) {
	unsigned char *data = new unsigned char[cam->getImageTypeSize( raspicam::RASPICAM_FORMAT_RGB )];
	cam->retrieve( data );
	std::ofstream outFile ( filename.c_str(), std::ios::binary );

	outFile<<"P6\n"<<cam->getWidth() <<" "<<cam->getHeight() <<" 255\n";
	outFile.write( (char*)data, filesize );
	cout<<"Image saved as "<<filename<<endl;
	delete data;
}

/*void testCB(void* x) {
	cout<<"Test Callback"<<endl;
	//return 0;
}*/

void camInit(raspicam::RaspiCam* Camera) {
	cout<<"Initing cam"<<endl;

	// override camera settings here
	Camera->setWidth(320);
	Camera->setHeight(240);
	//	Camera->setShutterSpeed(5000);
	//Camera->setExposure(raspicam::RASPICAM_EXPOSURE_VERYLONG);
	//uint tdata = 1;
	//Camera->setUserCallback(&testCB,(void*)&tdata);

	if ( !Camera->open()) {
		cerr<<"Error opening camera"<<endl;
		exit(1);
	}

	msleep(2000); //2s
	Camera->grab();
	msleep(100); // 0.1s
	Camera->grab();
	msleep(100); // 0.1s

	struct timespec start, finish;
	double elapsed;
	clock_gettime(CLOCK_MONOTONIC, &start);
	Camera->grab();
	clock_gettime(CLOCK_MONOTONIC, &finish);
	elapsed = (finish.tv_sec - start.tv_sec);
  elapsed += (finish.tv_nsec - start.tv_nsec) / 1000000000.0;
	cout<<"grab timing: "<<elapsed<<endl;

}


int main ( int argc,char **argv ) {

	piHiPri(50); // set high priority
	gpioInit();
	raspicam::RaspiCam* Camera = new raspicam::RaspiCam;
	camInit(Camera);

	uint filesize = Camera->getImageTypeSize( raspicam::RASPICAM_FORMAT_RGB );

	for(uint x=0; x<=9; x++) {
		struct timespec start, finish;
		double elapsed;
		clock_gettime(CLOCK_MONOTONIC, &start);
		flashRGBGrab( Camera, RED_LED, GRN_LED, BLU_LED );
		clock_gettime(CLOCK_MONOTONIC, &finish);
		elapsed = (finish.tv_sec - start.tv_sec);
		elapsed += (finish.tv_nsec - start.tv_nsec) / 1000000000.0;
		cout<<"flashRGBGrab timing: "<<elapsed<<endl;

		std::string fn = "picam-rgb"+std::to_string(x)+".ppm";
		writeOut( Camera, fn, filesize);
		//msleep(300);
	}

	cout << "Releasing heap memory and exiting....." << endl;
	return 0;
}
