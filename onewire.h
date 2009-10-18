//******************************************************************************
//*
//*     File: onewire.h
//*     declartions
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

#ifndef _ONEWIRE_H_
#define _ONEWIRE_H_

#include "inttypes.h"

typedef struct
{
    uint8_t devid;
    uint8_t sn0;
    uint8_t sn1;
    uint8_t sn2;
    uint8_t sn3;
    uint8_t sn4;
    uint8_t sn5;
    uint8_t crc;
} onewireId;

void onewireInit(void);

char onewireGetID(onewireId* id);

#endif /* _ONEWIRE_H_ */

