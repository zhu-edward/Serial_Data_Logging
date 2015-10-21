using namespace std;

#include "serial.h"
#include <string>
#include <time.h>
#include <iostream>
#include <fstream>

int findChksum (std::string &msg) {
	int chksum = 0;
	for (unsigned int i = 0; i <= (msg.length()-1); i++) {
		chksum ^= (int)msg[i];
	}
	return chksum;
}

int main (void) {
	// Initialize read buffer, and serial port parameters
	char buf[128];
	int fd;
	int baud = 9600;
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
	printf("main: Data is being logged into the file: ");
	cout << filename << endl;

	// Initialize serial port
	Serial serial;
	fd = serial.Initialize(serialport, baud);

	// Read from serial port and write to file
	if (fd > 0) {
		ofstream file (filename.c_str());
		if (file.is_open()) {
			printf("main: File opened successfully, logging data...\n");
			while (1) {
				if (serial.ReadUntil(fd, buf, '\n') != -1) {
					//printf("%s", buf);
					//file << buf;
					// Convert character array into string
					std::string buffer = buf;
					foundStart = buffer.find(startChar);
					foundEnd = buffer.find(endChar);
					if ((foundStart != std::string::npos) && (foundEnd != std::string::npos)) {
						std::string msg = buffer.substr(foundStart, (foundEnd-foundStart+1));
						std::string::size_type sz;
						int msgChksum = std::stoi(buffer.substr((foundEnd+1), (buffer.length()-foundEnd-2)), &sz);
						chksum = findChksum(msg);
						if (chksum == msgChksum) {
							msg = msg + '\n';
							//printf("%s", msg.c_str());
							//file << msg.c_str();
							//file << "Hello\n";
						}
					}
				}
			}
		}
		else {
			printf("main: File failed to open, exiting...\n");
			exit(EXIT_FAILURE);
		}	
	}
	else {
		printf("main: Initialization failed, exiting...\n");
		exit(EXIT_FAILURE);
	}

	return 0;
}