#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <termios.h>
#include <sys/ioctl.h>
#include <errno.h>
#include <getopt.h>

class Serial
{
public:
	Serial();
	int Initialize(const char* serialport, int baud);
	int ReadUntil(int fd, char* buf, char until);
	int ReadFromUntil(int fd, char* buf, char from, char until);
	int ReadBlock(int fd, char* buf);
};

