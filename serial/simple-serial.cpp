// File: SerialSimple.cpp
// Description: A simple program to use serial port
// Author: Yan Naing Aye

#include <stdio.h>
#include <termios.h> 
#include <unistd.h> 
#include <fcntl.h>
#include <string.h>

#define BAUDRATE B115200
//#define SERIAL_PATH "/dev/ttyUSB0"
#define SERIAL_PATH "/dev/ttyO4"

int main() {
    int fd; //file descriptor
    struct termios settings;
    char buffer_send[32] = "Test\r\n";
    char buffer_recv[32] = {0};

//open serial port
    printf("Opening serial port.\n");
    memset(&settings, 0, sizeof(settings));
	settings.c_iflag = 0;
	settings.c_oflag = 0;
	settings.c_cflag = CREAD | CLOCAL;//see termios.h for more information
    settings.c_cflag |= CS8;// 8 data bits
    settings.c_cflag |= PARENB;//no parity
    //1 stop bit if CSTOPB is not set
    settings.c_lflag = 0;
	settings.c_cc[VMIN] = 1;
	settings.c_cc[VTIME] = 0;
    fd = open(SERIAL_PATH, O_RDWR | O_NONBLOCK);
	if (fd == -1) {
        printf("Error in opening serial port!\n");
		return -1;
	}
    printf("Port opened successfully.\n");
    cfsetospeed(&settings, BAUDRATE);
	cfsetispeed(&settings, BAUDRATE);
    tcsetattr(fd, TCSANOW, &settings);
	int flags = fcntl(fd, F_GETFL, 0);
	fcntl(fd, F_SETFL, flags | O_NONBLOCK);
    
//write
    printf("Writing: %s\n",buffer_send);
    int write_ret = write(fd,buffer_send,strlen(buffer_send));
    printf("Written: %d\n",write_ret);
    
//wait a while 
    usleep(1000000);
    
//read
    int read_ret = read(fd,buffer_recv,strlen(buffer_send));
    printf("Received: %s\n",buffer_recv);
    printf("Read: %d\n",read_ret);
    
//close
    printf("Closing serial port.\n");
    close(fd);
	fd=-1;
	
    return 0;
}


