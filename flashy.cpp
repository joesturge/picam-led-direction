/*
 * PiCAM Main Project file
 */
#include <ctime>
#include <fstream>
#include <iostream>
#include <unistd.h>
#include <string>
#include <raspicam/raspicam.h>
#include "GPIOClass.h"

using namespace std;

void gpioInit(GPIOClass* r, GPIOClass* g, GPIOClass* b) {
	g->setdir_gpio("out");
  r->setdir_gpio("out");
  b->setdir_gpio("out");
}

void ledPulse(GPIOClass* led) {
	led->setval_gpio("1");
	usleep(3850);  // wait for 3.85ms seconds
	led->setval_gpio("0");
}

void flashGrab(raspicam::RaspiCam* cam, GPIOClass* led) {
	cout<<"Capturing image"<<endl;

	if (!cam->grab_dontwait()) {
		cerr<<"Error grabbing frame"<<endl;
	}
	ledPulse(led);
}

void flashRGBGrab(raspicam::RaspiCam* cam, GPIOClass* led1, GPIOClass* led2, GPIOClass* led3) {
	cout<<"Capturing RGB image"<<endl;

	if (!cam->grab_dontwait()) {
		cerr<<"Error grabbing frame"<<endl;
	}
	usleep(10000);
	ledPulse(led1);
	usleep(8970);
	ledPulse(led2);
	usleep(8970);
	ledPulse(led3);
	usleep(200000); // 50ms
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
	//Camera->setWidth(320);
	//Camera->setHeight(240);
	Camera->setShutterSpeed(50000);
	//Camera->setExposure(raspicam::RASPICAM_EXPOSURE_VERYLONG);
	//uint tdata = 1;
	//Camera->setUserCallback(&testCB,(void*)&tdata);

	if ( !Camera->open()) {
		cerr<<"Error opening camera"<<endl;
		exit(1);
	}

	usleep(2000000); //2s
	Camera->grab();
	usleep(100000); // 0.1s
	Camera->grab();
	usleep(100000); // 0.1s

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

  GPIOClass* rled = new GPIOClass("13");
	GPIOClass* gled = new GPIOClass("6");
	GPIOClass* bled = new GPIOClass("19");
	gpioInit(rled,gled,bled);
	raspicam::RaspiCam* Camera = new raspicam::RaspiCam;
	camInit(Camera);

	uint filesize = Camera->getImageTypeSize( raspicam::RASPICAM_FORMAT_RGB );

	/*flashGrab( Camera, rled );
	writeOut( Camera, "picam-red.ppm", filesize);

	flashGrab( Camera, gled );
	writeOut( Camera, "picam-gr.ppm", filesize);

	flashGrab( Camera, bled );
	writeOut( Camera, "picam-bl.ppm", filesize);*/

	for(uint x = 0; x<=9; x++) {
		flashRGBGrab( Camera, rled , gled, bled );
		std::string fn = "picam-rgb"+std::to_string(x)+".ppm";
		writeOut( Camera, fn, filesize);
		usleep(50000);
	}

	cout << "Releasing heap memory and exiting....." << endl;
	delete gled;
	delete rled;
	delete bled;
	//&g = NULL;
	//&r = NULL;
	//&b = NULL;
	return 0;
}
