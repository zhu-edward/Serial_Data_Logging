using namespace std;

#include "serial.h"

Serial::Serial () {}

int Serial::Initialize (const char* serialport, int baud) {
	struct termios toptions;
	int fd = open(serialport, O_RDWR | O_NOCTTY | O_NDELAY);

	if (fd == -1) {
		perror("Initialize: Unable to open serial port");
		return -1;
	}
    else {
        printf("Initialize: Serial port opened!\n");
    }

	if (tcgetattr(fd, &toptions) < 0) {
		perror("Initialize: Could not get term attributes");
		return -1;
	}
    else {
        printf("Initialize: Term attributes read!\n");
    }

	speed_t brate;

	switch(baud) {
    case 4800:   brate=B4800;   break;
    case 9600:   brate=B9600;   break;
	#ifdef B14400
    case 14400:  brate=B14400;  break;
	#endif
    case 19200:  brate=B19200;  break;
	#ifdef B28800
    case 28800:  brate=B28800;  break;
	#endif
    case 38400:  brate=B38400;  break;
    case 57600:  brate=B57600;  break;
    case 115200: brate=B115200; break;
    }

	cfsetispeed(&toptions, brate);
    cfsetospeed(&toptions, brate);

	// 8N1
    toptions.c_cflag &= ~PARENB;
    toptions.c_cflag &= ~CSTOPB;
    toptions.c_cflag &= ~CSIZE;
    toptions.c_cflag |= CS8;

    // no flow control
    toptions.c_cflag &= ~CRTSCTS;

    toptions.c_cflag |= CREAD | CLOCAL;  // turn on READ & ignore ctrl lines
    toptions.c_iflag &= ~(IXON | IXOFF | IXANY); // turn off s/w flow ctrl

    toptions.c_lflag &= ~(ICANON | ECHO | ECHOE | ISIG); // make raw
    toptions.c_oflag &= ~OPOST; // make raw

    // see: http://unixwiz.net/techtips/termios-vmin-vtime.html
    toptions.c_cc[VMIN]  = 0;
    toptions.c_cc[VTIME] = 20;
    
    if (tcsetattr(fd, TCSANOW, &toptions) < 0) {
        perror("Initialize: Couldn't set term attributes");
        return -1;
    }
    else {
        printf("Initialize: Term attributes set!\n");
    }

    return fd;
}

int Serial::ReadUntil (int fd, char* buf, char until) {
    char b[1];
    int i=0;
    do { 
        int n = read(fd, b, 1);  // read a char at a time
        if (n==-1) return -1;    // couldn't read
        if (n==0) {
            usleep( 10 * 1000 ); // wait 10 msec try again
            continue;
        }
        buf[i] = b[0]; i++;
    } while( b[0] != until );

    buf[i] = 0;  // null terminate the string
    return 0;
}

int Serial::ReadFromUntil (int fd, char* buf, char from, char until) {
    char b[1];
    int i = 0;
    int readEn = 0;
    do {
        int n = read(fd, b, 1);
        if (n == -1) return -1;
        if (n==0) {
            usleep( 10 * 1000 ); // wait 10 msec try again
            continue;
        }
        if (b[0] == from) {
            readEn = 1;
            buf[i] = b[0];
            i++;
        }
        if (readEn == 1) {
            buf[i] = b[0];
            i++;
        }
    } while(b[0] != until);

    buf[i] = 0;
    return 0;
}
    