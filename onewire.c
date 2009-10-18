//******************************************************************************
//*
//*     File: onewire.c
//*     implementation
//*
//*     1-Wire(tm) interface access. Read DS1990A i-Button(tm)
//*
//*
//*     Platform: AVR
//*     Tool: AVR-GCC
//*
//*     Version: 0.1b
//*
//*     Copyright (c) 2009, Ivan A-R <ivan@tuxotronic.org>
//*
//*     Permission is hereby granted, free of charge, to any person 
//*     obtaining  a copy of this software and associated documentation 
//*     files (the "Software"), to deal in the Software without restriction, 
//*     including without limitation the rights to use, copy, modify, merge, 
//*     publish, distribute, sublicense, and/or sell copies of the Software, 
//*     and to permit persons to whom the Software is furnished to do so, 
//*     subject to the following conditions:
//*
//*     The above copyright notice and this permission notice shall be included 
//*     in all copies or substantial portions of the Software.
//*
//*     THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, 
//*     EXPRESS  OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF 
//*     MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. 
//*     IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY 
//*     CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, 
//*     TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH 
//*     THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
//*
//******************************************************************************

/****
 * TODO write interrupt realisations (18.10.2009)
 *
 */


#include "onewire.h"

#include "config.h"

#include <util/delay.h>

#define WIRE0 DDRD |= (1<<PD2)
#define WIRE1 DDRD &= ~(1<<PD2)
#define WIRE  (PIND & (1<<PD2))

void onewireInit(void)
{
    DDRD  &= ~(1<<PD2);
    PORTD &= ~(1<<PD2);
}

void onewireSendByte(char v)
{
    char i;
    for(i=0; i<8; i++)
    {
        WIRE0;
        _delay_us(5);
        if(v & 1)
        {
            WIRE1;
            _delay_us(70);
        }
        else
        {
            _delay_us(70);
            WIRE1;
        }
        _delay_us(5);
        v >>= 1;
    }
}

char onewireReceiveByte(void)
{
    char i;
    char b = 0;
    for(i=0; i<8; i++)
    {
        WIRE0;
        _delay_us(5);
        WIRE1;
        _delay_us(35);
        b >>= 1;
        if(WIRE)
            b |= 0x80;
        _delay_us(35+5);
    }
    return b;
}

char onewireReset(void)
{
    uint16_t cnt;
    WIRE0;
    _delay_us(480+20);
    WIRE1;

    cnt = 0;
    while(WIRE != 0)
    {
        if((cnt++) > 60+10)
            return -1;
        _delay_us(1);
    }

    while(WIRE == 0)
        ;
    _delay_us(10);
}

char onewireGetID(onewireId* id)
{
    if(onewireReset())
        return -1;

    onewireSendByte(0x33);

    id->devid = onewireReceiveByte();
    id->sn0 = onewireReceiveByte();
    id->sn1 = onewireReceiveByte();
    id->sn2 = onewireReceiveByte();
    id->sn3 = onewireReceiveByte();
    id->sn4 = onewireReceiveByte();
    id->sn5 = onewireReceiveByte();
    id->crc = onewireReceiveByte();

    return 0;
}

