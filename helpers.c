#include "helpers.h"

#include <util/delay.h>

//#include "lcd.h"
#include <stdio.h>

void delay_ms(unsigned int ms)
/* delay for a minimum of <ms> */
{
        // we use a calibrated macro. This is more
        // accurate and not so much compiler dependent
        // as self made code.
        while(ms){
                _delay_ms(0.96);
                ms--;
                wdr();
        }
}

/*
void ERROR(uint8_t no, uint8_t code)
{
    lcd_clrscr();
    char outstr[20];
    snprintf_P(outstr, 20, PSTR("Error%d: 0x%02x"), no, code);
    lcd_puts(outstr);

    while(1)
    {
        for(uint8_t i=0; i<2; ++i)
        {
            LEDON;
            delay_ms(100);
            LEDOFF;
            delay_ms(200);
        }
        delay_ms(1000);
    }
}

*/

