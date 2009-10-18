/*
 * =====================================================================================
 *
 *       Filename:  main.c
 *
 *    Description:  Thermostat
 *
 *        Version:  1.0
 *        Created:  Fri Mar 14 17:43:49 MSK 2008
 *       Revision:  $Id$
 *       Compiler:  gcc
 *
 *         Author:  Ivan Alfiorov-Romamentiev (http://iar.spb.ru), ivalrom@gmail.com
 *        Company:  Freelancer
 *
 * =====================================================================================
 */

#include "config.h"

#include <string.h>
#include <stdio.h>

#include <avr/pgmspace.h>
#include <avr/interrupt.h>
#include <avr/sleep.h>
#include <util/delay.h>

#include "helpers.h"
#include "lcd.h"
#include "onewire.h"

static char _getHex(char v)
{
    if(v<10)
        return v + '0';
    else
        return v + 'A' - 10;
}

void putHex(uint8_t v)
{
    lcdPutc(_getHex(v>>4));
    lcdPutc(_getHex(v&0x0F));
}

void putHex16(uint16_t v)
{
    lcdPutc(_getHex((v>>12)&0x0F));
    lcdPutc(_getHex((v>>8)&0x0F));
    lcdPutc(_getHex((v>>4)&0x0F));
    lcdPutc(_getHex(v&0x0F));
}


#define WDATA0 (1<<PD4)
#define WDATA1 (1<<PD5)

void wiegandData(uint8_t mask)
{
    DDRD |= mask;
    _delay_us(200);
    DDRD &= ~mask;
    _delay_us(1800);
}

void wiegandSend(uint32_t val)
{
    char i;
    wiegandData(WDATA0);
    for(i=0; i<24; i++)
    {
        if(val & 1)
            wiegandData(WDATA1);
        else
            wiegandData(WDATA0);
        val >>= 1;
    }
    wiegandData(WDATA1);
}

onewireId id;

int main(void)
{
#ifdef WDT_ENABLE
    wdt_enable(WDTO_15MS);
#endif

    onewireInit();
    lcdInit();
    //lcdPuts_P(PSTR("Hello world"));

    for(;;)
    {
        wdr();
        if(onewireGetID(&id) == 0)
        {
            wiegandSend(((uint32_t)id.sn0) | ((uint32_t)id.sn1<<8) | ((uint32_t)id.sn2<<16) | ((uint32_t)id.sn3<<24));

            lcdGoto(0, 0);
            putHex(id.devid);
            lcdGoto(1, 0);
            putHex(id.sn5);
            putHex(id.sn4);
            putHex(id.sn3);
            putHex(id.sn2);
            putHex(id.sn1);
            putHex(id.sn0);
            lcdPutc(':');
            putHex(id.crc);
        }
        delay_ms(500);
    }
    return 0;
}

// vim: sw=4:ts=4:si:et
