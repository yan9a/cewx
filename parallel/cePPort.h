// File: cePPort.h
// Description: using parallel port on Linux
// WebSite: http://cool-emerald.blogspot.com
// MIT License (https://opensource.org/licenses/MIT)
// Copyright (c) 2018 Yan Naing Aye

// References
// https://www.htlinux.com/parallel_port_control_in_linux/
// http://cool-emerald.blogspot.com/2008/03/parallel-port-as-io.html

#ifndef CEPPORT_H
#define CEPPORT_H

namespace ce {

class cePPort {
private:
    int fd;
public:
    cePPort();
    ~cePPort();

    int Open();
    int Close();

    // read and write register p
    // where p = 0: data, 1: status, 2: control
    unsigned char Rd(int p);
    void Wr(unsigned char b,int p);

    // read and write bit n of register p
    // where p = 0: data, 1: status, 2: control
    bool Rdb(int n,int p); // read bit n of register p
    void Wrb(bool v,int n,int p); // write bit n of register p to v

    bool RdD(int n); // get bit n of data register
    void WrD(bool v,int n); // set bit n of data register to v
    bool RdS(int n); // get bit n of status register
    bool RdC(int n); // get bit n of control register
    void WrC(bool v,int n); // set bit n of control register to v
};

} // namespace ce 

#endif // CEPPORT_H