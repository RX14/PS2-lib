/*
  PS/2
  Created by RX14, September 20, 2014
  Unlicensed, do what you want with it!

  Most of the code is from whoever wrote the PS2 library on the Arduino Playground. Might be DGar but not sure...
*/

#include "PS2.h"

/*
  Initialise the PS2 library with the CLK and DATA pins.
*/
PS2::PS2(int CLK, int DATA)
{
    _CLK  = CLK;
    _DATA = DATA;
}

/*
  Sets the pin to the HIGH state.
*/
void PS2::gohi(int pin)
{
    pinMode(pin, INPUT);
    digitalWrite(pin, HIGH);
}

/*
  Sets the pin to the LOW state.
*/
void PS2::golo(int pin)
{
    pinMode(pin, OUTPUT);
    digitalWrite(pin, LOW);
}

/*
  Writes a unsigned char to the PS2 interface.
*/
void PS2::write(unsigned char data)
{
    unsigned char i;
    unsigned char parity = 1;

    // put pins in output mode
    gohi(_DATA);
    gohi(_CLK);
    delayMicroseconds(300);
    golo(_CLK);
    delayMicroseconds(300);
    golo(_DATA);
    delayMicroseconds(10);

    // start bit
    gohi(_CLK);

    // wait for device to take control of clock);
    while (digitalRead(_CLK) == HIGH)
        ;
    // clock is low, and we are clear to send data:

    for (i=0; i < 8; i++)
    {
        if (data & 0x01)
            gohi(_DATA);
        else
            golo(_DATA);

        // wait for clock cycle
        while (digitalRead(_CLK) == LOW)
            ;
        while (digitalRead(_CLK) == HIGH)
            ;

        parity = parity ^ (data & 0x01);
        data = data >> 1;
    }

    // parity
    if (parity)
        gohi(_DATA);
    else
        golo(_DATA);

    while (digitalRead(_CLK) == LOW)
        ;
    while (digitalRead(_CLK) == HIGH)
        ;

    // stop bit
    gohi(_DATA);
    delayMicroseconds(50);
    while (digitalRead(_CLK) == HIGH)
        ;

    // wait for device to switch modes
    while ((digitalRead(_CLK) == LOW) || (digitalRead(_DATA) == LOW))
        ;

    // put a hold on the incoming data.
    golo(_CLK);
}

unsigned char PS2::read()
{
    unsigned char data = 0x00;
    unsigned char i;
    unsigned char bit = 0x01;

    // start the clock
    gohi(_CLK);
    gohi(_DATA);
    delayMicroseconds(50);
    while (digitalRead(_CLK) == HIGH)
        ;
    delayMicroseconds(5);  // not sure why
    while (digitalRead(_CLK) == LOW) // eat start bit
        ;

    for (i=0; i < 8; i++) {
        while (digitalRead(_CLK) == HIGH)
            ;
        if (digitalRead(_DATA) == HIGH) {
            data = data | bit;
        }
        while (digitalRead(_CLK) == LOW)
            ;
        bit = bit << 1;
    }

    // eat parity bit, which we ignore
    while (digitalRead(_CLK) == HIGH)
        ;
    while (digitalRead(_CLK) == LOW)
        ;

    // eat stop bit
    while (digitalRead(_CLK) == HIGH)
        ;
    while (digitalRead(_CLK) == LOW)
        ;

    // put a hold on the incoming data.
    golo(_CLK);

    return data;
}

unsigned char PS2::cmd(unsigned char data)
{
    write(data);
    return read();
}

bool PS2::ack()
{
    return (unsigned char)read() == (unsigned char)0xFA;
}

bool PS2::cmd_ack(unsigned char data)
{
    write(data);
    return ack();
}
