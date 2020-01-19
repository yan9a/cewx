// File: cePPort.cpp
// Description: using parallel port on Linux
// WebSite: http://cool-emerald.blogspot.com
// MIT License (https://opensource.org/licenses/MIT)
// Copyright (c) 2018 Yan Naing Aye

// References
// https://www.htlinux.com/parallel_port_control_in_linux/
// http://cool-emerald.blogspot.com/2008/03/parallel-port-as-io.html

#include "cePPort.h"

// list of modes to access parallel port
#define CEPPORT_MA 0 // memory access mode
#define CEPPORT_PPDEV 1 // user space ppdev mode
// choose one of the above modes to control parallel port
#define CEPPORT_MODE CEPPORT_PPDEV 

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#if CEPPORT_MODE==CEPPORT_MA
    #include <sys/io.h>
    #define BASE 0x378
#else
    #include <sys/ioctl.h>
    #include <fcntl.h>
    #include <linux/parport.h>
    #include <linux/ppdev.h>
#endif

namespace ce {

cePPort::cePPort(){
    this->Open();
}

cePPort::~cePPort(){
    this->Close();
}

int cePPort::Open(){ 
    int r = 0;  
#if CEPPORT_MODE==CEPPORT_MA
    // Set permission bits of 4 ports starting from BASE
    r = ioperm(BASE, 4, 1);
    if(r!=0){
        perror("Could not set permissions on ports");        
    }
#else
    // Open the parallel port for reading and writing
    this->fd = open("/dev/parport0", O_RDWR);
    if(this->fd == -1)
    {
        perror("Could not open parallel port");
        return 1;
    }

    // Try to claim port
    r=ioctl(this->fd, PPCLAIM, NULL);
    if(r!=0)
    {
        perror("Could not claim parallel port");
        close(this->fd);
        return r;
    }

    // Set the Mode
    int mode = IEEE1284_MODE_BYTE;
    r=ioctl(this->fd, PPSETMODE, &mode);
    if(r!=0)
    {
        perror("Could not set mode");
        ioctl(this->fd, PPRELEASE);
        close(this->fd);
        return r;
    }

    // Set data pins to input/output
    int dir = 0x00;
    r = ioctl(this->fd, PPDATADIR, &dir);
    if(r!=0)
    {
        perror("Could not set parallel port direction");
        ioctl(this->fd, PPRELEASE);
        close(this->fd);
        return r;
    }
#endif 
    return r;
}

int cePPort::Close(){
    int r=0;
#if CEPPORT_MODE==CEPPORT_MA    
    // Clear permission bits of 4 ports starting from BASE
    r = ioperm(BASE, 4, 0);
    if(r!= 0){
        perror("Could not clear permissions on ports");        
    }
#else
    // Release and close the parallel port
    ioctl(this->fd, PPRELEASE);
    close(this->fd);
#endif 
    return r;
}

unsigned char cePPort::Rd(int p){
    unsigned char b = 0;
#if CEPPORT_MODE==CEPPORT_MA    
    b=inb(BASE+p);        
#else
    if(p == 0) ioctl(this->fd, PPRDATA, &b);
    else if(p == 1) ioctl(this->fd, PPRSTATUS, &b);
    else if(p == 2) ioctl(this->fd, PPRCONTROL, &b);     
#endif 
    return b;
}

void cePPort::Wr(unsigned char b,int p){
#if CEPPORT_MODE==CEPPORT_MA    
    outb(b,BASE+p);
#else
    if(p == 0) ioctl(this->fd, PPWDATA, &b);
    else if(p == 2) ioctl(this->fd, PPWCONTROL, &b);     
#endif   
}

bool cePPort::Rdb(int n,int p) // read bit n of register p
{
    unsigned char t = 1<<n;
    unsigned char b = this->Rd(p);    
    bool r = (t & b)?true:false;
    return r;
}

void cePPort::Wrb(bool v,int n,int p) // write bit n of register p to v
{
    unsigned char t=1<<n;
    unsigned char b = this->Rd(p); 
    b&=(~t);//clear
    if(v) b|=t;//set
    Wr(b,p);
}

bool cePPort::RdD(int n) // get bit n of data register
{
    return Rdb(n,0);
}

void cePPort::WrD(bool v,int n) // set bit n of data register to v
{
    Wrb(v,n,0);
}

bool cePPort::RdS(int n)  // get bit n of status register
{
    return Rdb(n,1);
}

bool cePPort::RdC(int n) // get bit n of control register
{
    return Rdb(n,2);
}

void cePPort::WrC(bool v,int n) // set bit n of control register to v
{
    Wrb(v,n,2);
}

} // namespace ce 
