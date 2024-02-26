//============================================================================
// Name        : I2C.cpp
// Author      : Marouane BEN ZINEB
// Description : main application
//============================================================================

#include <iostream>
#include "I2CDevice.h"
using namespace std;
using namespace EE513;

int main() {

	unsigned int busNumber = 1;
	unsigned int address = 0x68;
	I2CDevice RTC(busNumber, address);

	RTC.open();

	unsigned char seconds = RTC.readRegister(0x00);
	unsigned char minutes = RTC.readRegister(0x01);
	unsigned char hours = RTC.readRegister(0x02);


    cout << "Seconds: " << (int)seconds << endl;
    cout << "Minutes: " << (int)minutes << endl;
    cout << "Hours: " << (int)hours << endl;

    RTC.close();

    return 0;
}
