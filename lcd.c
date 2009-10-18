//******************************************************************************
//*
//*     File: lcd.c
//*     implementation
//*
//*     4-bit, 6 line access to lcd controller HD44780
//*     Support 2 and 4 line display
//*
//*
//*     Platform: AVR and other
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

#include "lcd.h"

#include "config.h"

/**
 * @block Hardware depend part
 */
#include <util/delay.h>
#include <avr/pgmspace.h>

#define LCD_GPIO PORTB
#define LCD_RS (1<<4)
#define LCD_E  (1<<5)

#define LCD_DSHIFT 0
#define LCD_D0 (1<<(LCD_DSHIFT+0))
#define LCD_D1 (1<<(LCD_DSHIFT+1))
#define LCD_D2 (1<<(LCD_DSHIFT+2))
#define LCD_D3 (1<<(LCD_DSHIFT+3))
#define LCD_DMASK (LCD_D0 | LCD_D1 | LCD_D2 | LCD_D3)

#define E_DELAY 10

#define LINE0 0x00
#define LINE1 0x40
#define LINE2 0x14
#define LINE3 0x54

/*
 * Elementary hardware depend commands
 */

//#define CMD_E_DELAY() { volatile unsigned char i; for(i=0; i<E_DELAY; i++) ; }
#define CMD_SLEEP_MS(ms) _delay_ms(ms)

#define CMD_E_MARK LCD_GPIO |= LCD_E
#define CMD_E_RELEASE LCD_GPIO &= ~LCD_E

#define CMD_RS0 LCD_GPIO &= ~LCD_RS
#define CMD_RS1 LCD_GPIO |= LCD_RS

#define CMD_DATA(x) LCD_GPIO = (LCD_GPIO & ~LCD_DMASK) | ((x)<<LCD_DSHIFT)

#define CMD_CLEAN { LCD_GPIO &= ~(LCD_RS | LCD_E | LCD_DMASK); }

#define CMD_CONF_GPIO { DDRB |= LCD_DMASK | LCD_RS | LCD_E; }

static inline void CMD_E_DELAY(void)
{
    /*
    asm volatile (" nop ");
    volatile unsigned char i;
    for(i=0; i<E_DELAY; i++) ;
    */
}

static inline void pulse_e(void)
{
    CMD_E_MARK;
    CMD_E_DELAY();
    CMD_E_RELEASE;
    CMD_E_DELAY(); // ???
}

static inline void wait_ready(void)
{
    _delay_ms(1);
    /*
    uint16_t i;
    for(i=0; i<2000; i++) 
        asm volatile ("nop");
    */
}


static inline void _lcd_init(void)
{
}

void lcdPuts_P(char* str)
{
    char c;
    while((c = pgm_read_byte(str++)) != 0)
        lcdPutc(c);
}

/**
 * @block hardware undepend part
 */


#ifdef WRAPEN
static uint8_t cur_col;
static uint8_t cur_row;
#endif

void lcdInit(void)
{
    _lcd_init();

    CMD_CONF_GPIO;
    CMD_CLEAN;
    CMD_SLEEP_MS(16); // 16 ms

    CMD_DATA(3); // 8bit mode
    pulse_e();
    CMD_SLEEP_MS(5); // 4992 us

    pulse_e(); // Repeat 8bit mode
    CMD_SLEEP_MS(1); // 64 us

    pulse_e(); // Third repeat
    CMD_SLEEP_MS(1); // 64 us

    CMD_DATA(2); // Change to 4bit mode (0x20)
    pulse_e();

    lcdCmd(LCD_CMD_INIT4);
    /*
    lcdCmd(0x24);
    lcdCmd(0x09);
    lcdCmd(0x20);
    */

    lcdCmd(0x08);
    lcdCmd(0x01);
    lcdCmd(0x06);
    lcdCmd(0x0C);

    lcdCmd(0x80);
    /*
    lcdCmd(0x02);
    */
    /*
    lcdCmd(LCD_SET_DM | LCD_DM_DISPLAY_ON);
    lcdCmd(LCD_SET_INCREMENT_MODE);
    */

#ifdef WRAPEN
    cur_col = 0;
    cur_row = 0;
#endif
}

void lcdCmd(unsigned char cmd)
{
    wait_ready();
    CMD_RS0; // controll command
    CMD_DATA(cmd >> 4);
    pulse_e();
    CMD_DATA(cmd & 0x0F);
    pulse_e();
}

void lcdData(unsigned char cmd)
{
    wait_ready();
    CMD_RS1; // data
    CMD_DATA(cmd >> 4);
    pulse_e();
    CMD_DATA(cmd & 0x0F);
    pulse_e();
}

void lcdGoto(unsigned char row, unsigned char col)
{
    char line_base;
    switch(row)
    {
        default:
            line_base = LINE0;
            break;
        case 1:
            line_base = LINE1;
            break;
#if (LCD_ROWS == 4)
        case 2:
            line_base = LINE2;
            break;
        case 3:
            line_base = LINE3;
            break;
#endif
    }
    lcdCmd((line_base + col) | LCD_SET_DDRAM_ADDRESS);
#ifdef WRAPEN
    cur_col = col;
    cur_row = row;
#endif
}

#ifdef WRAPEN
void lcdPutc(char c)
{
    if(c == '\n')
    {
        cur_col = 0;
        if(cur_row < LCD_ROWS-1)
            cur_row++;
        lcdGoto(cur_row, cur_col);
    }
    else
    {
        if(cur_col >= LCD_COLS)
        {
            cur_col = 0;
            if(cur_row < LCD_ROWS-1)
                cur_row++;
            lcdGoto(cur_row, cur_col);
        }
        lcdData(c);
        cur_col++;
    }
}

#else
void lcdPutc(char c)
{
    lcdData(c);
}
#endif


void lcdPuts(char* str)
{
    char c;
    while((c = *str++) != 0)
        lcdPutc(c);
}


