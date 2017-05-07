/*
 * PiCAM Main Project file
 */
#include <ctime>
#include <fstream>
#include <iostream>
#include <unistd.h>
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
	usleep(200000);  // wait for 0.2 seconds
	led->setval_gpio("0");
}

void flashGrab(raspicam::RaspiCam* cam, GPIOClass* led) {
	cout<<"Capturing image"<<endl;
	led->setval_gpio("1");
	if (!cam->grab()) {
		cerr<<"Error grabbing frame"<<endl;
	}
	led->setval_gpio("0");
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


void camInit(raspicam::RaspiCam* Camera) {
	cout<<"Initing cam"<<endl;

	// override camera settings here
	Camera->setWidth(320);
	Camera->setHeight(240);

	if ( !Camera->open()) {
		cerr<<"Error opening camera"<<endl;
		exit(1);
	}

	usleep(2000000); //2s
	Camera->grab();
	usleep(100000); // 0.1s
	Camera->grab();
	usleep(100000); // 0.1s
}


int main ( int argc,char **argv ) {

  cout<<"Starting"<<endl;
  GPIOClass* rled = new GPIOClass("13");
	GPIOClass* gled = new GPIOClass("6");
	GPIOClass* bled = new GPIOClass("19");
	gpioInit(rled,gled,bled);
	raspicam::RaspiCam* Camera = new raspicam::RaspiCam;
	camInit(Camera);

	uint filesize = Camera->getImageTypeSize( raspicam::RASPICAM_FORMAT_RGB );

	flashGrab( Camera, rled );
	writeOut( Camera, "picam-red.ppm", filesize);

	flashGrab( Camera, gled );
	writeOut( Camera, "picam-gr.ppm", filesize);

	flashGrab( Camera, bled );
	writeOut( Camera, "picam-bl.ppm", filesize);

	cout << "Releasing heap memory and exiting....." << endl;
	delete gled;
	delete rled;
	delete bled;
	//&g = NULL;
	//&r = NULL;
	//&b = NULL;
	return 0;
}
