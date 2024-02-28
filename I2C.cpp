//============================================================================
// Name        : I2C.cpp
// Author      : Marouane BEN ZINEB
// Description : main application
//============================================================================

#include <iostream>
#include <iomanip>
#include <sstream>
#include "I2CDevice.h"
#include <ctime>
#include <string>
using namespace std;
using namespace EE513;

class RTC : public I2CDevice {

	private:
		unsigned int secAddress = 0x00;
		unsigned int minAddress = 0x01;
		unsigned int hourAddress = 0x02;

		unsigned int dayAddress = 0x04;
		unsigned int monthAddress = 0x05;
		unsigned int yearAddress = 0x06;

		unsigned int temperatureAddress = 0x11;
		unsigned int temperatureQuarterAddress = 0x12;

	public:
		RTC(unsigned int bus, unsigned int device) : I2CDevice(bus, device) {

		}

	void displayTimeDate() {

		unsigned char seconds = readRegister(secAddress);
		unsigned char minutes = readRegister(minAddress);
		unsigned char hours = readRegister(hourAddress);

		cout << "--------------------------------------------------------" << endl;

		cout << "Time (24H format): "
				<< std::hex
				<< std::setw(2) << std::setfill('0') << static_cast<int>(hours)
				<< ":"
				<< std::setw(2) << std::setfill('0') << static_cast<int>(minutes)
				<< ":"
				<< std::setw(2) << std::setfill('0') << static_cast<int>(seconds) << endl;

		unsigned char days = readRegister(dayAddress);
		unsigned char month = readRegister(monthAddress);
		unsigned char year = readRegister(yearAddress);

		cout << "Date in the format DD-MM-YY: "
				<< std::hex
				<< std::setw(2) << std::setfill('0') << static_cast<int>(days) << "-"
				<< std::setw(2) << std::setfill('0') << static_cast<int>(month) << "-"
				<< std::setw(2) << std::setfill('0') << static_cast<int>(year) << endl;

		cout << "--------------------------------------------------------" << endl;

	}

	void displayTemperature() {
		char temperature = readRegister(temperatureAddress);
		char temperatureQuarter = readRegister(temperatureQuarterAddress);
		cout << "Temperature: " << static_cast<int>(temperature) << "." << static_cast<int>(temperatureQuarter) << "°C" << endl;

		cout << "--------------------------------------------------------" << endl;
	}

	void setCurrentTimeDate() {
		time_t currentTime = time(nullptr);
		struct tm* timeinfo = localtime(&currentTime);

		//retrieving the current time and date
		unsigned int s = timeinfo->tm_sec;
		unsigned int min = timeinfo->tm_min;
		unsigned int h = timeinfo->tm_hour;

		// each variable is considered as a hex number and converted to decimal as the write function converts automatically the decimal value to hex
		min = std::stoi(std::to_string(min), nullptr, 16);
		s = std::stoi(std::to_string(s), nullptr, 16);
		h = std::stoi(std::to_string(h), nullptr, 16);

		unsigned int d = (timeinfo->tm_mday);
		unsigned int month = (timeinfo->tm_mon + 1); // starts from 0 (0-11)
		unsigned int y = (timeinfo->tm_year - 100); //starts from 1900 so 2024 - 1900 = 124 => we remove 100

		d = std::stoi(std::to_string(d), nullptr, 16);
		month = std::stoi(std::to_string(month), nullptr, 16);
		y = std::stoi(std::to_string(y), nullptr, 16);

		//Time
		writeRegister(secAddress, s);
		writeRegister(minAddress, min);
		writeRegister(hourAddress, h);
		//Date
		writeRegister(dayAddress, d);
		writeRegister(monthAddress, month);
		writeRegister(yearAddress, y);


		cout << "DATE SET TO: " 
		<< std::setw(2) << std::setfill('0') << d << "-" 
		<< std::setw(2) << std::setfill('0') << month << "-" 
		<< std::setw(2) << std::setfill('0') << y << endl;


		cout << "DATE SET TO: " 
		<< std::setw(2) << std::setfill('0') << h << ":" 
		<< std::setw(2) << std::setfill('0') << min << ":" 
		<< std::setw(2) << std::setfill('0') << s << endl;

		cout << "--------------------------------------------------------" << endl;
	}

};

int main() {

	unsigned int busNumber = 1;
	unsigned int address = 0x68;

	RTC RTC(busNumber, address);

	RTC.open();

	RTC.displayTimeDate();


	RTC.setCurrentTimeDate();

	RTC.displayTimeDate();

	RTC.displayTemperature();



    RTC.close();

    return 0;
}
