using namespace std;

#include "serial.h"
#include <string>
#include <time.h>
#include <iostream>
#include <fstream>
#include <ostream>

int findChksum (std::string &msg) {
	int chksum = 0;
	for (unsigned int i = 0; i <= (msg.length()-1); i++) {
		chksum ^= (int)msg[i];
	}
	return chksum;
}

int main (void) {
	// Initialize read buffer, and serial port parameters
	int fd;
	const int baud = 9600;
	char serialport[] = "/dev/ttyACM0";

	// Define the delimiters of message
	std::string startChar = "$";
	std::string endChar = "!";
	std::size_t foundStart, foundEnd;
	int chksum = 0;

	// Setup file name for log file
	time_t rawtime;
	struct tm *timeinfo;
	time(&rawtime);
	timeinfo = localtime(&rawtime);
	std::string timestr (asctime(timeinfo));
	std::string logPath = "/home/edward/Documents/DataLogging/log_files/";
	std::string filename = logPath + "log_" + timestr.substr(0,timestr.length()-1) + ".dat";

	// Display file name
	cout << "main: Data is being logged into the file: ";
	cout << filename << endl;

	// Initialize serial port
	Serial serial;
	fd = serial.Initialize(serialport, baud);

	// Read from serial port and write to file
	if (fd > 0) {
		std::ofstream file (filename.c_str());
		if (file.is_open()) {
			cout << "main: File opened successfully, logging data...\n";
			while (1) {
				char buf[127];
				if (serial.ReadBlock(fd, buf) != -1) {
					//file << buf;
					cout << buf;
					//printf("%s", buf);
					//cout << sizeof(buf) << endl;
					// Convert character array into string
					std::string buffer = buf;
					//cout << buffer;
					foundStart = buffer.find_first_of(startChar);
					foundEnd = buffer.find_first_of(endChar, foundStart+1);
					//cout << foundStart << "|" << foundEnd << endl;
					if ((foundStart != std::string::npos) && (foundEnd != std::string::npos)) {
						std::string msg = buffer.substr(foundStart, (foundEnd-foundStart+1));
						std::string::size_type sz;
						int msgChksum = std::stoi(buffer.substr((foundEnd+1), (buffer.length()-foundEnd-2)), &sz);
						chksum = findChksum(msg);
						if (chksum == msgChksum) {
							msg = msg + '\n';
							//file << msg;
							//cout << msg;
						}
					}
				}
			}
		}
		else {
			cout << "main: File failed to open, exiting...\n";
			exit(EXIT_FAILURE);
		}	
	}
	else {
		cout << "main: Initialization failed, exiting...\n";
		exit(EXIT_FAILURE);
	}

	return 0;
}