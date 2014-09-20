/*
  PS/2
  Created by RX14, September 20, 2014
  Unlicensed, do what you want with it!
*/

#ifndef PS2_h_RX14
#define PS2_h_RX14

#include "Arduino.h"

class PS2
{
public:
    PS2(int CLK, int DATA);

    void write(unsigned char data);
    unsigned char read();

    unsigned char cmd(unsigned char data);
    bool ack();
    bool cmd_ack(unsigned char data);

    void gohi(int pin);
    void golo(int pin);

private:
    int _CLK, _DATA;
}

#endif
