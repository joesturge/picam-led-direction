# PiCAM Direction Sensing Project

Group Project for Processing PiCam Images from LED Flashes and Displaying a Direction based on this.

```
git clone <url>
git submodule update --init
```

## RaspiCAM build

```
cd raspicam
mkdir build
cd build
cmake ..
make -j4
sudo make install
sudo ldconfig
```

## GPIO Setup

Test program build

```
make testgpio
sudo ./testGPIO
```

## Main Program Build

```
make teststill
sudo ./testStill
```




