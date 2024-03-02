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
#include <chrono>
#include <thread>
using namespace std;
using namespace EE513;

class RTC : public I2CDevice {

	private:

		unsigned int secAddress = 0x00;
		unsigned int minAddress = 0x01;
		unsigned int hourAddress = 0x02;

		unsigned int dayNameAddress = 0x03;
		unsigned int dateAddress = 0x04;
		unsigned int monthAddress = 0x05;
		unsigned int yearAddress = 0x06;

		unsigned int alarm1SecAddress = 0x07;
		unsigned int alarm1MinAddress = 0x08;
		unsigned int alarm1HourAddress = 0x09;
		unsigned int alarm1DateAddress = 0x0A;

		unsigned int alarm2MinAddress = 0x0B;
		unsigned int alarm2HourAddress = 0x0C;
		unsigned int alarm2DateAddress = 0x0D;

		unsigned int controlAddress = 0x0E;
		unsigned int statusAddress = 0x0F;

		unsigned int temperatureAddress = 0x11;
		unsigned int temperatureFractionAddress = 0x12;

	public:
		RTC(unsigned int bus, unsigned int device) : I2CDevice(bus, device) {

		}

	// converting to Binary Coded Decimal (BCD)
	int convertToBCD(int value) {
		return ((value / 10 * 16) + (value % 10));
	}

	void displayTimeDate() {

		unsigned int seconds = readRegister(secAddress);
		unsigned int minutes = readRegister(minAddress);
		unsigned int hours = readRegister(hourAddress);

		cout << "Current Time (24H format): "
				<< std::hex
				<< std::setw(2) << std::setfill('0') << (hours)
				<< ":"
				<< std::setw(2) << std::setfill('0') << (minutes)
				<< ":"
				<< std::setw(2) << std::setfill('0') << (seconds) << endl;

		unsigned int dayName = readRegister(dayNameAddress);
		unsigned int date = readRegister(dateAddress);
		unsigned int month = readRegister(monthAddress);
		unsigned int year = readRegister(yearAddress);

		cout << "Current Date (DD-MM-YY): ";
				switch (dayName) {
					case 1:
						cout << "Monday ";
						break;
					case 2:
						cout << "Tuesday ";
						break;
					case 3:
						cout << "Wednesday ";
						break;
					case 4:
						cout << "Thursday ";
						break;
					case 5:
						cout << "Friday ";
						break;
					case 6:
						cout << "Saturday ";
						break;
					case 7:
						cout << "Sunday ";
						break;
					default:
						cout << "Invalid day ";
						break;
				}
				cout << std::hex
				<< std::setw(2) << std::setfill('0') << (date) << "-"
				<< std::setw(2) << std::setfill('0') << (month) << "-"
				<< std::setw(2) << std::setfill('0') << (year) << endl;
		cout << "--------------------------------------------------------" << endl;

	}

	void displayTemperature() {

		// https://arduinodiy.wordpress.com/2015/11/10/the-ds3231-rtc-temperature-sensor/
		float temp;
    	float fraction;

     	int8_t msb  = readRegister(temperatureAddress); // most significant bit
    	uint8_t lsb = readRegister(temperatureFractionAddress); //least significant bit
		
		fraction = ((lsb >> 6) & 0b00000011) * 0.25; // the & operation is used to get the 2 least significant bits of the fraction after shifting the lsb by 6 to the right which shifts the 2 bits to the least position
		// 0.25 is the resolution

		temp = msb+fraction;

		if (msb & 0b10000000) { // checks for sign bit, if it is 1 then temperature is negative (bit 7 is the sign bit, 1 for negative, 0 for positive)
		//After adding this portion of code, i found out that int8_t takes into account the sign bit, so no need to check for the sign bit
			temp = temp * -1;
		}

		cout << "TEMPERATURE: " << (temp) << "°C" << endl;
		cout << "--------------------------------------------------------" << endl;

	}

	void setTimeDate(unsigned int sec, unsigned int min, unsigned int hour, unsigned int date, unsigned int month, unsigned int year) {

		cout << "TIME SET TO: " 
		<< std::dec
		<< std::setw(2) << std::setfill('0') << hour << ":"
		<< std::setw(2) << std::setfill('0') << min << ":"
		<< std::setw(2) << std::setfill('0') << sec << endl;

		cout << "DATE SET TO: "
		<< std::setw(2) << std::setfill('0') << date << "-"
		<< std::setw(2) << std::setfill('0') << month << "-"
		<< std::setw(2) << std::setfill('0') << year << endl;
		
		cout << "--------------------------------------------------------" << endl;

		//Time
		writeRegister(secAddress, convertToBCD(sec));
		writeRegister(minAddress, convertToBCD(min));
		writeRegister(hourAddress, convertToBCD(hour));
		//Date
		writeRegister(dateAddress, convertToBCD(date));
		writeRegister(monthAddress, convertToBCD(month));
		writeRegister(yearAddress, convertToBCD(year));


	}

	void setCurrentTimeDate() { // https://cplusplus.com/reference/ctime/tm/
	// https://www.tutorialspoint.com/c_standard_library/c_function_localtime.htm
		time_t currentTime = time(nullptr);
		struct tm* timeinfo = localtime(&currentTime);

		//retrieving the current time and date
		unsigned int s = timeinfo->tm_sec;
		unsigned int min = timeinfo->tm_min;
		unsigned int h = timeinfo->tm_hour;

		unsigned int dayName = timeinfo->tm_wday; // day name
		unsigned int d = (timeinfo->tm_mday); // date
		unsigned int month = (timeinfo->tm_mon + 1); // starts from 0 (0-11)
		unsigned int y = (timeinfo->tm_year - 100); //starts from 1900 so 2024 - 1900 = 124 => we remove 100

		
		//y = std::stoi(std::to_string(y), nullptr, 16) // another way i found that works
		// the variable is considered as a hex number and converted to decimal as the write function converts automatically the decimal value to hex

		//Time
		writeRegister(secAddress, convertToBCD(s));
		writeRegister(minAddress, convertToBCD(min));
		writeRegister(hourAddress, convertToBCD(h));
		//Date
		writeRegister(dayNameAddress, dayName); //no need to convert the dayName as the values between 1 and 7 are already in BCD
		writeRegister(dateAddress, convertToBCD(d));
		writeRegister(monthAddress, convertToBCD(month));
		writeRegister(yearAddress, convertToBCD(y));


		cout << "DATE SET TO: ";
		switch (timeinfo->tm_wday) {
			case 1:
				cout << "Monday ";
				break;
			case 2:
				cout << "Tuesday ";
				break;
			case 3:
				cout << "Wednesday ";
				break;
			case 4:
				cout << "Thursday ";
				break;
			case 5:
				cout << "Friday ";
				break;
			case 6:
				cout << "Saturday ";
				break;
			case 7:
				cout << "Sunday ";
				break;
			default:
				cout << "Invalid day ";
				break;
		}
		cout << std::dec <<
		std::setw(2) << std::setfill('0') << timeinfo->tm_mday << "-" 
		<< std::setw(2) << std::setfill('0') << timeinfo->tm_mon + 1 << "-" 
		<< std::setw(2) << std::setfill('0') << timeinfo->tm_year - 100 << endl;


		cout << "TIME SET TO: " 
		<< std::setw(2) << std::setfill('0') << timeinfo->tm_hour << ":" 
		<< std::setw(2) << std::setfill('0') << timeinfo->tm_min << ":" 
		<< std::setw(2) << std::setfill('0') << timeinfo->tm_sec << endl;

		cout << "--------------------------------------------------------" << endl;
	}


	    // Set Alarm 1
    void setAlarm1(bool dy_dt, unsigned char day_date, unsigned char hour, unsigned char minute, unsigned char second, unsigned char mask) {
        cout <<"ALARM 1 SET ON: "
		<< std::dec
		<< std::setw(2) << std::setfill('0') << (int)hour << ":"
		<< std::setw(2) << std::setfill('0') << (int)minute << ":"
		<< std::setw(2) << std::setfill('0') << (int)second << " on ";

		if (mask == 0b0000) { //Alarm when date, hours, minutes, and seconds match
			if (dy_dt) { // =1 => day of the week		
				switch (day_date) {
					case 1:
						cout << "Sunday" << endl;
						break;
					case 2:
						cout << "Monday" << endl;
						break;
					case 3:
						cout << "Tuesday" << endl;
						break;
					case 4:
						cout << "Wednesday" << endl;
						break;
					case 5:
						cout << "Thursday" << endl;
						break;
					case 6:
						cout << "Friday" << endl;
						break;
					case 7:
						cout << "Saturday" << endl;
						break;
					default:
						cout << "Invalid day" << endl;
						break;
				}
				day_date = day_date | 0b01000000; // Set the DY/DT bit by OR-ing with 0b10000000 (bit 6)
			}
			else { // =0 => date of the month
				cout << "the" << std::setw(2) << std::setfill('0') << (int)day_date << "th" << endl;
				day_date = day_date | 0b00000000; // Set the DY/DT bit by OR-ing with 0b00000000 (bit 6)
			}
		}

		if (mask == 0b1000) { // Alarm when hours, minutes, and seconds match
    		day_date |= 0b10000000; // Set the A1M4 bit by OR-ing with 0b10000000 (bit 7)
			cout << "ALARM MODE: Hours, minutes, and seconds matching" << endl;
		}

		if (mask == 0b1100) { // Alarm when minutes and seconds match
			day_date |= 0b10000000; // Set the A1M4 bit by OR-ing with 0b10000000 (bit 7)
			hour |= 0b10000000; // Set the A1M3 bit by OR-ing with 0b10000000 (bit 7)
			cout << "ALARM MODE: Minutes and seconds matching" << endl;
		}

		if (mask == 0b1110) { // Alarm when seconds match
			day_date |= 0b10000000; // Set the A1M4 bit by OR-ing with 0b10000000 (bit 7)
			hour |= 0b10000000; // Set the A1M3 bit by OR-ing with 0b10000000 (bit 7)
			minute |= 0b10000000; // Set the A1M2 bit by OR-ing with 0b10000000 (bit 7)
			cout << "ALARM MODE: Seconds matching" << endl;
		}

		if (mask == 0b1111) { // Alarm every second
			day_date |= 0b10000000; // Set the A1M4 bit by OR-ing with 0b10000000 (bit 7)
			hour |= 0b10000000; // Set the A1M3 bit by OR-ing with 0b10000000 (bit 7)
			minute |= 0b10000000; // Set the A1M2 bit by OR-ing with 0b10000000 (bit 7)
			second |= 0b10000000; // Set the A1M1 bit by OR-ing with 0b10000000 (bit 7)
			cout << "ALARM MODE: Every second" << endl;
		}

		this->writeRegister(alarm1SecAddress,  convertToBCD(second) ); // A1M1
		this->writeRegister(alarm1MinAddress, convertToBCD(minute) ); // A1M2
		this->writeRegister(alarm1HourAddress, convertToBCD(hour) );   // A1M3
		this->writeRegister(alarm1DateAddress, convertToBCD(day_date) ); // A1M4
		cout << "--------------------------------------------------------" << endl;
    }

	void isAlarm1Triggered () {
		while (true) 
		{		
			unsigned char status = readRegister(statusAddress);
			if (status & 0b00000001) {
				cout << "STATUS: ALARM 1 TRIGGERED" << endl;
				enableInterruptAlarm1();

				cout << "UNTRIGGERING ALARM 1 IN 5 SECONDS..." << endl;
				std::this_thread::sleep_for(std::chrono::milliseconds(5000));
				writeRegister(statusAddress, (status & 0b11111110) ); // untriggering the alarm 1 (A1F bit is cleared by writing 0 to it)
				writeRegister(controlAddress, (readRegister(controlAddress)) & 0b11111110); //(A1IE bit is cleared by writing 0 to it)
				cout << "ALARM 1 UNTRIGGERED" << endl;

				break;
			} 
			else {
				cout << "STATUS: ALARM 1 NOT TRIGGERED" << endl;
				std::this_thread::sleep_for(std::chrono::milliseconds(300));
			}
		}
	}

	void enableInterruptAlarm1() {
		unsigned int control = readRegister(controlAddress);
		control |= 0b00000001; // set the INTCN and A1IE bits
		writeRegister(controlAddress, control);
	}

	void setAlarm2(bool dy_dt, unsigned char day_date, unsigned char hour, unsigned char minute, unsigned char mask) {

        cout <<"ALARM 2 SET ON: "
		<< std::dec
		<< std::setw(2) << std::setfill('0') << (int)hour << ":"
		<< std::setw(2) << std::setfill('0') << (int)minute << " on ";

		if (mask == 0b000) { // Alarm when date, hours and minutes match
			if (dy_dt) { // =1 => day of the week
				switch (day_date) {
					case 1:
						cout << "Sunday" << endl;
						break;
					case 2:
						cout << "Monday" << endl;
						break;
					case 3:
						cout << "Tuesday" << endl;
						break;
					case 4:
						cout << "Wednesday" << endl;
						break;
					case 5:
						cout << "Thursday" << endl;
						break;
					case 6:
						cout << "Friday" << endl;
						break;
					case 7:
						cout << "Saturday" << endl;
						break;
					default:
						cout << "Invalid day" << endl;
						break;
				}
				day_date = day_date | 0b01000000; // Set the DY/DT bit by OR-ing with 0b10000000 (bit 6)
			}
			else { // =0 => date of the month
				cout << "the" << std::setw(2) << std::setfill('0') << (int)day_date << "th" << endl;
				day_date = day_date | 0b00000000; // Set the DY/DT bit by OR-ing with 0b00000000 (bit 6)
			}

		}
		
		if (mask == 0b100) { // Alarm when hours and minutes match
			day_date |= 0b10000000; // Set the A2M4 bit by OR-ing with 0b10000000 (bit 7)
			cout << "ALARM MODE: Hours and minutes matching" << endl;
		}

		if (mask == 0b110) { // Alarm when minutes match
			day_date |= 0b10000000; // Set the A2M4 bit by OR-ing with 0b10000000 (bit 7)
			hour |= 0b10000000; // Set the A2M3 bit by OR-ing with 0b10000000 (bit 7)
			cout << "ALARM MODE: Minutes matching" << endl;
		}

		if (mask == 0b111) { // Alarm once per minute
			day_date |= 0b10000000; // Set the A2M4 bit by OR-ing with 0b10000000 (bit 7)
			hour |= 0b10000000; // Set the A2M3 bit by OR-ing with 0b10000000 (bit 7)
			minute |= 0b10000000; // Set the A2M2 bit by OR-ing with 0b10000000 (bit 7)
			cout << "ALARM MODE: Seconds matching" << endl;
		}

		this->writeRegister(alarm2MinAddress, convertToBCD(minute) ); // A2M2
		this->writeRegister(alarm2HourAddress, convertToBCD(hour) );   // A2M3
		this->writeRegister(alarm2DateAddress, convertToBCD(day_date) ); // A2M4
		cout << "--------------------------------------------------------" << endl;

	}


	void isAlarm2Triggered () {
		while (true) 
		{		
			unsigned char status = readRegister(statusAddress);
			if (status & 0b00000010) {
				cout << "STATUS: ALARM 2 TRIGGERED" << endl;
				enableInterruptAlarm2();

				cout << "UNTRIGGERING ALARM 2 IN 5 SECONDS..." << endl;
				std::this_thread::sleep_for(std::chrono::milliseconds(5000));
				writeRegister(statusAddress, (status & 0b11111101) ); // untriggering the alarm 2 // untriggering the alarm 2 (A2F bit is cleared by writing 0 to it)
				writeRegister(controlAddress, (readRegister(controlAddress)) & 0b11111101); //(A2IE bit is cleared by writing 0 to it)
				cout << "ALARM 2 UNTRIGGERED" << endl;
			
				break;
			} 
			else {
				cout << "STATUS: ALARM 2 NOT TRIGGERED" << endl;
				std::this_thread::sleep_for(std::chrono::milliseconds(300));
			}
		}
	}

	void enableInterruptAlarm2() {
		unsigned int control = readRegister(controlAddress);
		control |= 0b00000010; // set the INTCN and A2IE bits
		writeRegister(controlAddress, control );
	}

};

int main() {

	unsigned int busNumber = 1;
	unsigned int address = 0x68;

	RTC RTC(busNumber, address);


	cout << "--------------------------------------------------------" << endl;
	cout << " TESTING FUNCTIONS" << endl;
	cout << "--------------------------------------------------------" << endl;

	//RTC.setCurrentTimeDate();

	// second, minute, hour, date, month, year
	//RTC.setTimeDate(30, 32, 02, 29, 02, 24);

	RTC.setCurrentTimeDate();

	RTC.displayTimeDate();

	//RTC.displayTemperature();

	//mask = 0b0000 => all components must match 
	//mask = 0b1111 => every second
	//mask = 0b1000 => hours, minutes, and seconds must match
	//mask = 0b1100 => minutes and seconds must match
	//mask = 0b1110 => seconds must match
	//RTC.setAlarm1(0, 02, 16, 30, 59, 0b1110); // param1 = 0 => date of the month / = 1 => day of the week
	// (dy_dt, day_date, hour, minute, second, mask)

	//RTC.isAlarm1Triggered();


	//mask = 0b000 => all components must match
	//mask = 0b111 => every minute
	//mask = 0b100 => hours and minutes must match
	//mask = 0b110 => minutes must match
	RTC.setAlarm2(0, 30, 16, 29, 0b111); // param1 = 0 => date of the month / = 1 => day of the week
	// (dy_dt, min, hour, day_date, mask)

	RTC.isAlarm2Triggered();



    RTC.close();

    return 0;
}
