//File: frame.h
//Description: Byte stuffing- sending and receiving frames
//Author: Yan Naing Aye
//WebSite: http://cool-emerald.blogspot.com
//MIT License (https://opensource.org/licenses/MIT)
//Copyright (c) 2018 Yan Naing Aye

// References
// http://coolemerald.blogspot.com/2009/09/crc-calculation-in-vb-and-c.html

#ifndef  FRAME_H
#define FRAME_H

#include <stdio.h>
#define STX 0x02
#define ETX 0x03
#define DLE 0x10

#define TX_BUF_SIZE 128
#define RX_BUF_SIZE 128
enum RX_STATE { IGNORE,RECEIVING,ESCAPE,RXCRC1,RXCRC2 };

class Frame {    
    RX_STATE rState;
protected:
    int TxN;//number of transmitting bytes
    int RxN;//number of receiving bytes
    char tb[TX_BUF_SIZE];//transmit buffer
    char rb[RX_BUF_SIZE];//receiving data
public:
    Frame();
    int setTxFrame(char* d,int n);
    unsigned int CRC16CCITT_Calculate(char* s,unsigned char len,unsigned int crc);
    int getTxN();
    int getRxN();
    int receiveRxFrame(char c);//get receiving frame from received char
    char* getTxBuf();
    char* getRxBuf();
};

Frame::Frame():TxN(0),RxN(0),rState(IGNORE){}

char* Frame::getTxBuf(){
    return tb;
}

char* Frame::getRxBuf(){
    return rb;
}

//Prepare transmitting frame
int Frame::setTxFrame(char* d,int n)
{
    unsigned int txcrc=0xFFFF;//initialize crc
    char c;
    int i=0,j=0;
    tb[i++]=STX;//start of frame
    for(j=0;j < n;j++) {
        c=d[j];
        if((c==STX)||(c==ETX)||(c==DLE)) tb[i++]=(DLE);
        tb[i++]=c;
    }
    tb[i++]=(ETX);//end of frame

    txcrc=CRC16CCITT_Calculate(d,n,txcrc);//calculate crc
    tb[i++]=txcrc & 0xFF;
    tb[i++]=(txcrc >> 8) & 0xFF;
    TxN=i;
    return TxN;
}

//Inputs
//s : pointer to input char string
//len: string len (maximum 255)
//crc: initial CRC value

//Output
//Returns calculated CRC
unsigned int Frame::CRC16CCITT_Calculate(char* s,unsigned char len,unsigned int crc)
{
    //CRC Order: 16
    //CCITT(recommendation) : F(x)= x16 + x12 + x5 + 1
    //CRC Poly: 0x1021
    //Operational initial value:  0xFFFF
    //Final xor value: 0
    unsigned char i,j;
    for(i=0;i < len;i++,s++) {
        crc^=((unsigned int)( *s ) & 0xFF) << 8;
        for(j=0;j<8;j++) {
            if(crc & 0x8000) crc=(crc << 1)^0x1021;
            else crc <<=1;
        }
    }
    return (crc & 0xFFFF);//truncate last 16 bit
}

//get number of transmitting bytes
int Frame::getTxN()
{
    return TxN;
}

//get number of transmitting bytes
int Frame::getRxN()
{
    return RxN;
}

//process receiving char
int Frame::receiveRxFrame(char c)
{
    static char b;
    unsigned int crc;
    unsigned int rxcrc=0xFFFF;//initialize CRC
    switch(rState){
        case IGNORE:
            if(c==STX) { rState=RECEIVING;RxN=0;}
            break;
        case RECEIVING:
            if(c==STX) { rState=RECEIVING;RxN=0;}
            else if(c==ETX){rState=RXCRC1;}
            else if(c==DLE){ rState=ESCAPE; }
            else { rb[RxN++]=c; }
            break;
        case ESCAPE:
            rb[RxN++]=c; rState=RECEIVING;
            break;
        case RXCRC1:
            b=c; rState=RXCRC2;
            break;
        case RXCRC2:
            rState=IGNORE;
            crc=( (int)c << 8 | ((int)b & 0xFF) ) & 0xFFFF;//get received crc
            rxcrc=CRC16CCITT_Calculate(rb,RxN,rxcrc);//calculate crc
            //printf("crc: %x  rxcrc:%x \n",crc,rxcrc);
            if(rxcrc==crc){return RxN;}//if crc is correct            
            else {RxN=0;}//discard the frame

            break;
    }
    return 0;
}

class Frame2:public Frame {
    char Dt[20];//transmitting data
public:
    Frame2();
    void printTxFrame();
    void printRxFrame();
    void printRxData();
    void setTxData(float x,float y,float z,float b,float t);
};

Frame2::Frame2():Frame(),Dt(""){}

//Print out frame content
void Frame2::printTxFrame()
{
    printf("Tx frame buffer: ");
    for(int j=0;j < TxN;j++) printf("%02X ",(unsigned char)tb[j]);
    printf("\n");
}

//Print out frame content
void Frame2::printRxFrame()
{
    printf("Rx data buffer: ");
    for(int j=0;j < RxN;j++) printf("%02X ",(unsigned char)rb[j]);
    printf("\n");
}

//Set transmitting data
void Frame2::setTxData(float x,float y,float z,float b,float t)
{
    *(float*)(Dt)=x;
    *(float*)(Dt+4)=y;
    *(float*)(Dt+8)=z;
    *(float*)(Dt+12)=b;
    *(float*)(Dt+16)=t;
    Frame::setTxFrame(Dt,20);
}

//Print out received data
void Frame2::printRxData()
{
    float x,y,z,b,t;
    x=*(float*)(Dt);
    y=*(float*)(Dt+4);
    z=*(float*)(Dt+8);
    b=*(float*)(Dt+12);
    t=*(float*)(Dt+16);
    printf("Rx data: %f %f %f %f %f \n",x,y,z,b,t);
}

#endif // FRAME_H
