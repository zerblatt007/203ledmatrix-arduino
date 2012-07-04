
char dummyvar;

/*
  Hacked by: Roger Storvik at gmail

  203 led matrix display from dealextreme with atmega88pa microcontroller, 
  set up as a front door doorbellbutton connected with two wires to a Friedland 8-12v AC doorbellsystem.
  It has the extra eeprom removed to get access to two extra controllerpins for the CapSense solid state button
  and a wire soldered onto pin 32 PD2 that is connected to a relay that shortcircuits the AC wires to make the DingDong

  This project is a hack, made possible by this hack:
  http://www.digitalspirit.org/blog/index.php/post/2011/05/18/Hacking-d-un-%C3%A9cran-%C3%A0-Led-de-DealExtreme

  Homepage for this dingdong project: http://code.google.com/p/203ledmatrix-arduino/

*/

#include <CapSense.h>
#include <MsTimer2.h>
#include <TimerOne.h>
#include <avr/pgmspace.h>
#include <avr/wdt.h>
#include <EEPROM.h>

#define _DDRB   0
#define _DDRC   1
#define _DDRD   2
#define _PORTB  3
#define _PORTC  4
#define _PORTD  5

#define RELAY 2
#define _PC4 18
#define _PC5 19

#define BUTTONlevel 30

#define MATRIX_COL_COUNT    29
#define MATRIX_LINE_COUNT   7

#define LONGBIT(x) ((unsigned long)0x00000001 << (x)) 
#define BIT(x)	(0x01 << (x)) 
#define bit_get(p,m) ((p) & (m)) 
#define bit_set(p,m) ((p) |= (m)) 

unsigned char matrix[29][7][6] PROGMEM = {

    // 0
    {
        { 4,        4,      0,      4,      0,      0   },
        { 2,        4,      0,      2,      0,      0   },
        { 1,        4,      0,      1,      0,      0   },
        { 0,        4,      128,    0,      0,      128 },
        { 0,        4,      32,     0,      0,      32  },
        { 128,      4,      0,      128,    0,      0   },
        { 64,       4,      0,      64,     0,      0   },
    },

    // 1
    {
        { 4,         0,     64,     0,      0,      64 },
        { 2,         0,     64,     0,      0,      64 },
        { 1,         0,     64,     0,      0,      64 },
        { 0,         0,     64+128, 0,      0,      64 },
        { 0,         0,     64+32,  0,      0,      64 },
        { 128,       0,     64,     0,      0,      64 },
        { 64,        0,     64,     0,      0,      64 }
    },

    // 2
    {
        { 4,         0,     64,     4,      0,      0 },
        { 2,         0,     64,     2,      0,      0 },
        { 1,         0,     64,     1,      0,      0 },
        { 0,         0,     64+128, 0,      0,      128 },
        { 0,         0,     64+32,  0,      0,      32 },
        { 128,       0,     64,     128,    0,      0 },
        { 64,        0,     64,     64,     0,      0 }
    },

    // 3
    {
        { 4+8,         0,     0,      8,      0,      0 },
        { 2+8,         0,     0,      8,      0,      0 },
        { 1+8,         0,     0,      8,      0,      0 },
        { 0+8,         0,     128,    8,      0,      0 },
        { 0+8,         0,     32,     8,      0,      0 },
        { 128+8,       0,     0,      8,      0,      0 },
        { 64+8,        0,     0,      8,      0,      0 }
    },

    // 4
    {
        { 4+8,         0,     0,      4,      0,      0 },
        { 2+8,         0,     0,      2,      0,      0 },
        { 1+8,         0,     0,      1,      0,      0 },
        { 0+8,         0,     128,    0,      0,      128 },
        { 0+8,         0,     32,     0,      0,      32 },
        { 128+8,       0,     0,      128,      0,      0 },
        { 64+8,        0,     0,      64,      0,      0 }
    },

    // 5
    {
        { 4+16,         0,     0,      16,      0,      0 },
        { 2+16,         0,     0,      16,      0,      0 },
        { 1+16,         0,     0,      16,      0,      0 },
        { 0+16,         0,     128,    16,      0,      0 },
        { 0+16,         0,     32,     16,      0,      0 },
        { 128+16,       0,     0,      16,      0,      0 },
        { 64+16,        0,     0,      16,      0,      0 }
    },

    // 6
    {
        { 4+16,         0,     0,      4,      0,      0 },
        { 2+16,         0,     0,      2,      0,      0 },
        { 1+16,         0,     0,      1,      0,      0 },
        { 0+16,         0,     128,    0,      0,      128 },
        { 0+16,         0,     32,     0,      0,      32 },
        { 128+16,       0,     0,      128,      0,      0 },
        { 64+16,        0,     0,      64,      0,      0 }
    },

    // 7
    {
        { 4+32,         0,     0,      32,      0,      0 },
        { 2+32,         0,     0,      32,      0,      0 },
        { 1+32,         0,     0,      32,      0,      0 }, // Souci sous 5v
        { 0+32,         0,     128,    32,      0,      0 },       
        { 0+32,         0,     32,     32,      0,      0 },
        { 128+32,       0,     0,      32,      0,      0 },
        { 64+32,        0,     0,      32,      0,      0 }
    },

    // 8
    {
        { 4+32,         0,     0,      4,      0,      0 },
        { 2+32,         0,     0,      2,      0,      0 },
        { 1+32,         0,     0,      1,      0,      0 },
        { 0+32,         0,     128,    0,      0,      128 },
        { 0+32,         0,     32,     0,      0,      32 },
        { 128+32,       0,     0,      128,      0,      0 },
        { 64+32,        0,     0,      64,      0,      0 }
    },

    // 9
    {
        { 4,         1,     0,      0,      1,      0 },
        { 2,         1,     0,      0,      1,      0 },
        { 1,         1,     0,      0,      1,      0 },
        { 0,         1,     128,    0,      1,      0 },
        { 0,         1,     32,     0,      1,      0 },
        { 128,       1,     0,      0,      1,      0 },
        { 64,        1,     0,      0,      1,      0 }
    },

    // 10
    {
        { 4,         1,     0,      4,      0,      0 },
        { 2,         1,     0,      2,      0,      0 },
        { 1,         1,     0,      1,      0,      0 },
        { 0,         1,     128,    0,      0,      128 },
        { 0,         1,     32,     0,      0,      32 },
        { 128,       1,     0,      128,      0,      0 },
        { 64,        1,     0,      64,      0,      0 }
    },

    // 11
    {
        { 4,         8,     0,      0,      8,      0 },
        { 2,         8,     0,      0,      8,      0 },
        { 1,         8,     0,      0,      8,      0 },
        { 0,         8,     128,    0,      8,      0 },
        { 0,         8,     32,     0,      8,      0 },
        { 128,       8,     0,      0,      8,      0 },
        { 64,        8,     0,      0,      8,      0 }
    },

    // 12
    {
        { 4,         8,     0,      4,      0,      0 },
        { 2,         8,     0,      2,      0,      0 },
        { 1,         8,     0,      1,      0,      0 },
        { 0,         8,     128,    0,      0,      128 },
        { 0,         8,     32,     0,      0,      32 },
        { 128,       8,     0,      128,    0,      0 },
        { 64,        8,     0,      64,     0,      0 }
    },

    // 13
    {
        { 4,         2,     0,      0,      2,      0 },
        { 2,         2,     0,      0,      2,      0 },
        { 1,         2,     0,      0,      2,      0 },
        { 0,         2,     128,    0,      2,      0 },
        { 0,         2,     32,     0,      2,      0 },
        { 128,       2,     0,      0,      2,      0 },
        { 64,        2,     0,      0,      2,      0 }
    },

    // 14
    {
        { 4,         2,     0,      4,      0,      0 },
        { 2,         2,     0,      2,      0,      0 },
        { 1,         2,     0,      1,      0,      0 },
        { 0,         2,     128,    0,      0,      128 },
        { 0,         2,     32,     0,      0,      32 },
        { 128,       2,     0,      128,    0,      0 },
        { 64,        2,     0,      64,     0,      0 }
    },

    // 15
    {
        { 4+64,         0,     0,      64,      0,      0 },
        { 2+64,         0,     0,      64,      0,      0 },
        { 1+64,         0,     0,      64,      0,      0 },
        { 64,         0,     128,    64,      0,      0 },
        { 64,         0,     32,     64,      0,      0 },
        { 128+64,       0,     0,      64,      0,      0 },
        { 0,        8+2,     0,      0,      8,      0 }
    },

    // 16
    {
        { 4+64,         0,     0,      4,      0,      0 },
        { 2+64,         0,     0,      2,      0,      0 },
        { 1+64,         0,     0,      1,      0,      0 },
        { 0+64,         0,     128,    0,      0,      128 },
        { 0+64,         0,     32,     0,      0,      32 },
        { 128+64,       0,     0,      128,    0,      0 },
        { 0,        8+2,     0,      0,     2,      0 }
    },

    // 17
    {
        { 4+128,         0,     0,      128,      0,      0 },
        { 2+128,         0,     0,      128,      0,      0 },
        { 1+128,         0,     0,      128,      0,      0 },
        { 128,         0,     128,    128,      0,      0 },
        { 128,         0,     32,     128,      0,      0 },
        { 0,       1+8,     0,      128,      1,      0 },
        { 0,        1+2,     0,      0,      1,      0 }
    },

    // 18
    {
        { 4+128,         0,     0,      4,      0,      0 },
        { 2+128,         0,     0,      2,      0,      0 },
        { 1+128,         0,     0,      1,      0,      0 },
        { 128,         0,     128,    0,      0,      128 },
        { 128,         0,     32,     0,      0,      32 },
        { 0,       1+8,     0,      0,      8,      0 },
        { 0,        1+2,     0,      0,      2,      0 }
    },

    // 19
    {
        { 4,         0,     32,      0,      0,      32 },
        { 2,         0,     32,      0,      0,      32 },
        { 1,         0,     32,      0,      0,      32 },
        { 0,         0,     128+32,    0,      0,      32 },
        { 32,         1,     0,     32,      0,      0 },
        { 32,       8,     0,      32,      0,      0 },
        { 32,        2,     0,      32,      0,      0 }
    },

    // 20
    {
        { 4,         0,     32,      4,      0,      0 },
        { 2,         0,     32,      2,      0,      0 },
        { 1,         0,     32,      1,      0,      0 },
        { 0,         0,     128+32,    0,      0,      128 },
        { 32,         1,     0,     0,      1,      0 },
        { 32,       8,     0,      0,      8,      0 },
        { 32,        2,     0,      0,      2,      0 }
    },

    // 21
    {
        { 4,         0,     128,      0,      0,      128 },
        { 2,         0,     128,      0,      0,      128 },
        { 1,         0,     128,      0,      0,      128 },
        { 16+32,     0,     0,    16,      0,      0 },
        { 16,         1,     0,     16,      0,      0 },
        { 16,       8,     0,      16,      0,      0 },
        { 16,        2,     0,      16,      0,      0 }
    },

    // 22
    {
        { 4,         0,     128,      4,      0,      0 },
        { 2,         0,     128,      2,      0,      0 },
        { 1,         0,     128,      1,      0,      0 },
        { 16+32,     0,     0,    32,      0,      0 },
        { 16,         1,     0,     0,      1,      0 },
        { 16,       8,     0,      0,      8,      0 },
        { 16,        2,     0,      0,      2,      0 }
    },

    // 23
    {
        { 4+1,      0,     0,      1,      0,      0 },
        { 2+1,      0,     0,      1,      0,      0 },
        { 8+16,     0,     0,      8,      0,      0 },
        { 8+32,     0,     0,    8,      0,      0 },
        { 8,        1,     0,     8,      0,      0 },
        { 8,        8,     0,      8,      0,      0 },
        { 8,        2,     0,      8,      0,      0 }
    },

    // 24
    {
        { 4+1,      0,     0,      4,      0,      0 },
        { 2+1,      0,     0,      2,      0,      0 },
        { 8+16,     0,     0,      16,      0,      0 },
        { 8+32,     0,     0,    32,      0,      0 },
        { 8,        1,     0,     0,      1,      0 },
        { 8,        8,     0,      0,      8,      0 },
        { 8,        2,     0,      0,      2,      0 }
    },

    // 25
    {
        { 4+2,      0,     0,      2,      0,      0 },
        { 8,      0,     64,      0,      0,      64 },
        { 16,     0,     64,      0,      0,      64 },
        { 32,     0,     64,    0,      0,      64 },
        { 0,        1,     64,     0,      0,      64 },
        { 0,        8,     64,      0,      0,      64 },
        { 0,        2,     64,      0,      0,      64 }
    },

    // 26
    {
        { 4+2,      0,     0,      4,      0,      0 },
        { 8,      0,     64,      8,      0,      0 },
        { 16,     0,     64,      16,      0,      0 },
        { 32,     0,     64,    32,      0,      0 },
        { 0,        1,     64,     0,      1,      0 },
        { 0,        8,     64,      0,      8,      0 },
        { 0,        2,     64,      0,      2,      0 }
    },

    // 27
    {
        { 0,      4,     64,      0,     4,      0 },
        { 8,      4,     0,      0,      4,      0 },
        { 16,     4,     0,      0,      4,      0 },
        { 32,     4,     0,    0,      4,      0 },
        { 0,        4+1,     0,     0,      4,      0 },
        { 0,        4+8,     0,      0,      4,      0 },
        { 0,        4+2,     0,      0,      4,      0 }
    },

    // 28
    {
        { 0,      4,     64,      0,      0,      64 },
        { 8,      4,     0,      8,      0,      0 },
        { 16,     4,     0,      16,      0,      0 },
        { 32,     4,     0,    32,      0,      0 },
        { 0,      4+1,   0,     0,      1,      0 },
        { 0,      4+8,   0,      0,      8,      0 },
        { 0,      4+2,   0,         0,      2,      0 }
    }
};

// standard ascii 5x7 font
// defines ascii characters 0x20-0x7F (32-127)
static unsigned char Font5x7[] PROGMEM = {
	0x00, 0x00, 0x00, 0x00, 0x00,// (space)
	0x00, 0x00, 0x5F, 0x00, 0x00,// !
	0x00, 0x07, 0x00, 0x07, 0x00,// "
	0x14, 0x7F, 0x14, 0x7F, 0x14,// #
	0x24, 0x2A, 0x7F, 0x2A, 0x12,// $
	0x23, 0x13, 0x08, 0x64, 0x62,// %
	0x36, 0x49, 0x55, 0x22, 0x50,// &
	0x00, 0x05, 0x03, 0x00, 0x00,// '
	0x00, 0x1C, 0x22, 0x41, 0x00,// (
	0x00, 0x41, 0x22, 0x1C, 0x00,// )
	0x08, 0x2A, 0x1C, 0x2A, 0x08,// *
	0x08, 0x08, 0x3E, 0x08, 0x08,// +
	0x00, 0x50, 0x30, 0x00, 0x00,// ,
	0x08, 0x08, 0x08, 0x08, 0x08,// -
	0x00, 0x60, 0x60, 0x00, 0x00,// .
	0x20, 0x10, 0x08, 0x04, 0x02,// /
	0x3E, 0x51, 0x49, 0x45, 0x3E,// 0
	0x00, 0x42, 0x7F, 0x40, 0x00,// 1
	0x42, 0x61, 0x51, 0x49, 0x46,// 2
	0x21, 0x41, 0x45, 0x4B, 0x31,// 3
	0x18, 0x14, 0x12, 0x7F, 0x10,// 4
	0x27, 0x45, 0x45, 0x45, 0x39,// 5
	0x3C, 0x4A, 0x49, 0x49, 0x30,// 6
	0x01, 0x71, 0x09, 0x05, 0x03,// 7
	0x36, 0x49, 0x49, 0x49, 0x36,// 8
	0x06, 0x49, 0x49, 0x29, 0x1E,// 9
	0x00, 0x36, 0x36, 0x00, 0x00,// :
	0x00, 0x56, 0x36, 0x00, 0x00,// ;
	0x00, 0x08, 0x14, 0x22, 0x41,// <
	0x14, 0x14, 0x14, 0x14, 0x14,// =
	0x41, 0x22, 0x14, 0x08, 0x00,// >
	0x02, 0x01, 0x51, 0x09, 0x06,// ?
	0x32, 0x49, 0x79, 0x41, 0x3E,// @
	0x7E, 0x11, 0x11, 0x11, 0x7E,// A
	0x7F, 0x49, 0x49, 0x49, 0x36,// B
	0x3E, 0x41, 0x41, 0x41, 0x22,// C
	0x7F, 0x41, 0x41, 0x22, 0x1C,// D
	0x7F, 0x49, 0x49, 0x49, 0x41,// E
	0x7F, 0x09, 0x09, 0x01, 0x01,// F
	0x3E, 0x41, 0x41, 0x51, 0x32,// G
	0x7F, 0x08, 0x08, 0x08, 0x7F,// H
	0x00, 0x41, 0x7F, 0x41, 0x00,// I
	0x20, 0x40, 0x41, 0x3F, 0x01,// J
	0x7F, 0x08, 0x14, 0x22, 0x41,// K
	0x7F, 0x40, 0x40, 0x40, 0x40,// L
	0x7F, 0x02, 0x04, 0x02, 0x7F,// M
	0x7F, 0x04, 0x08, 0x10, 0x7F,// N
	0x3E, 0x41, 0x41, 0x41, 0x3E,// O
	0x7F, 0x09, 0x09, 0x09, 0x06,// P
	0x3E, 0x41, 0x51, 0x21, 0x5E,// Q
	0x7F, 0x09, 0x19, 0x29, 0x46,// R
	0x46, 0x49, 0x49, 0x49, 0x31,// S
	0x01, 0x01, 0x7F, 0x01, 0x01,// T
	0x3F, 0x40, 0x40, 0x40, 0x3F,// U
	0x1F, 0x20, 0x40, 0x20, 0x1F,// V
	0x7F, 0x20, 0x18, 0x20, 0x7F,// W
	0x63, 0x14, 0x08, 0x14, 0x63,// X
	0x03, 0x04, 0x78, 0x04, 0x03,// Y
	0x61, 0x51, 0x49, 0x45, 0x43,// Z
	0x00, 0x00, 0x7F, 0x41, 0x41,// [
	0x02, 0x04, 0x08, 0x10, 0x20,// "\"
	0x41, 0x41, 0x7F, 0x00, 0x00,// ]
	0x04, 0x02, 0x01, 0x02, 0x04,// ^
	0x40, 0x40, 0x40, 0x40, 0x40,// _
	0x00, 0x01, 0x02, 0x04, 0x00,// `
	0x20, 0x54, 0x54, 0x54, 0x78,// a
	0x7F, 0x48, 0x44, 0x44, 0x38,// b
	0x38, 0x44, 0x44, 0x44, 0x20,// c
	0x38, 0x44, 0x44, 0x48, 0x7F,// d
	0x38, 0x54, 0x54, 0x54, 0x18,// e
	0x08, 0x7E, 0x09, 0x01, 0x02,// f
	0x08, 0x14, 0x54, 0x54, 0x3C,// g
	0x7F, 0x08, 0x04, 0x04, 0x78,// h
	0x00, 0x44, 0x7D, 0x40, 0x00,// i
	0x20, 0x40, 0x44, 0x3D, 0x00,// j
	0x00, 0x7F, 0x10, 0x28, 0x44,// k
	0x00, 0x41, 0x7F, 0x40, 0x00,// l
	0x7C, 0x04, 0x18, 0x04, 0x78,// m
	0x7C, 0x08, 0x04, 0x04, 0x78,// n
	0x38, 0x44, 0x44, 0x44, 0x38,// o
	0x7C, 0x14, 0x14, 0x14, 0x08,// p
	0x08, 0x14, 0x14, 0x18, 0x7C,// q
	0x7C, 0x08, 0x04, 0x04, 0x08,// r
	0x48, 0x54, 0x54, 0x54, 0x20,// s
	0x04, 0x3F, 0x44, 0x40, 0x20,// t
	0x3C, 0x40, 0x40, 0x20, 0x7C,// u
	0x1C, 0x20, 0x40, 0x20, 0x1C,// v
	0x3C, 0x40, 0x30, 0x40, 0x3C,// w
	0x44, 0x28, 0x10, 0x28, 0x44,// x
	0x0C, 0x50, 0x50, 0x50, 0x3C,// y
	0x44, 0x64, 0x54, 0x4C, 0x44,// z
	0x00, 0x08, 0x36, 0x41, 0x00,// {
	0x00, 0x00, 0x7F, 0x00, 0x00,// |
	0x00, 0x41, 0x36, 0x08, 0x00,// }
	0x08, 0x08, 0x2A, 0x1C, 0x08,// ->
	0x08, 0x1C, 0x2A, 0x08, 0x08 // <-
};


boolean buffer[MATRIX_COL_COUNT + 6][MATRIX_LINE_COUNT] = {0};
char dispStr[80];
char message[80];

prog_char constStr[] = "Her bor Penny & Roger  \0";
int shiftPos = 0;
int dispStrPos = 0;

byte buttonHold = 0;

// Capsense Def PC4 & PC5 (18,19)
CapSense cs_pc45 = CapSense(_PC4,_PC5);

void (*restart)(void) = 0;

void DingDong() {
  Serial.println("on");
  digitalWrite(RELAY, HIGH);
  delay(500);
  Serial.println("off");
  digitalWrite(RELAY, LOW);
  delay(500);
}

void setup() {
  strcpy(dispStr,constStr); // copy constant string to displaybuffer
  wdt_enable(WDTO_4S);  // watchdog init
  Serial.begin(115200);
  readEEpromMsg();

  Timer1.initialize(1000);
  Timer1.attachInterrupt(displayBuffer);
  MsTimer2::set(50, messageShift);
  MsTimer2::start();
  pinMode(RELAY,OUTPUT);
  digitalWrite(RELAY, LOW);
//  cs_pc45.set_CS_AutocaL_Millis(0xFFFFFFFF);
  cs_pc45.set_CS_Timeout_Millis(300);

  Serial.println("\n-- led-DingDong");
  Serial.println("d - dingdong");
  Serial.println("w - message");
  Serial.println("! - boot");
 
}


void loop() {
  wdt_reset(); // we are alive!
  if (Serial.available()){
    switch (Serial.read()){  
      case 'd':
        DingDong();
        break;         
      case 'w':
        changeMessage();
        break;         
      case '!':
        restart();
        break;
      }
      wdt_reset(); // we are alive!
  }
  long button =  cs_pc45.capSense(30);
  if ((buttonHold == 0) & (button >= 0)) {
//      buttonHold = 1;
    wdt_reset(); // we are alive!
    Serial.println("-- ding ");
    DingDong();
  }
//  if ((buttonHold == 1) & (button < 0)) {
//    buttonHold = 0;
//  }    
  Serial.println(".");
}

void burnEEpromMsg(const char* string) {
  byte numBytes= strlen(string) + 1;
  for (byte i=0; i < numBytes; i++) { // i = eeprom address
    EEPROM.write(i,string[i]);
  }
}

void readEEpromMsg() {  
  boolean zeroFound = false;
  for (byte i=0; i < 80; i++) { // i = eeprom address
    message[i] = EEPROM.read(i);
    if ((message[i] == 0) & (!zeroFound)) {
      zeroFound = true;
    }
  }
  if (!zeroFound) return;
  if (strlen(message) > 0) {
    strcpy(dispStr,message);
    return;
  }
}

void changeMessage() {
  byte message_size = 0;
  

  Serial.println("Type msg - ENTER to exit :");
  
  while (true) {
    wdt_reset(); // we are alive!
    
    if (!Serial.available()){
      continue;
    }
    
    char receivedBuffer = Serial.read(); // Read a char from serial

    // Enter key pressed? Then we exit and save to EEPROM
    if (receivedBuffer == 13) {
      if (sizeof(message) > 0) {
        message[message_size++]=0;
        burnEEpromMsg(message);
        strcpy(dispStr,message);
      }
      break;
    }
    if
    message[message_size++] = receivedBuffer;
    Serial.print(receivedBuffer);

    // Test max string size
    if (message_size == sizeof(message) - 2) { // extra headroom for \0
      Serial.println("\nMax Message size reached !");
      break;
    }
  }

}

void messageShift() {
    shiftLeft();
    if (shiftPos++ > 4) { //Char spacing
      writeChar(29,dispStr[dispStrPos++]);
      shiftPos = 0;
      if (dispStr[dispStrPos] == 0) {
        dispStrPos = 0;
      }
    }
}

void shiftLeft () {

  for (int y = 0; y < 7; y++) {
    for (int x = 0; x < 34; x++) {
      buffer[x][y]=buffer[x+1][y];
    }
  }
  
}

void writeChar(int col, unsigned char c) {

    char temp = 0;
    char b = 0;
    
    for (char i = 0; i < 5; i++) {
        temp = pgm_read_byte(Font5x7 + (c - 0x20) * 5 + i);
        
        for (b = 0; b < 7; b++) {
            if (temp & (1 << b)) {
                buffer[col + i][b]= 1;
            } 
        }
    }
}
 
inline void displayBuffer() {
  for (int y = 0; y < 7; y++) {
    for (int x = 0; x < 29; x++) {
      if (buffer[x][y]) { 
        setLeds(x,y);
      }
    }
  }
  clearLeds();
}

inline void setLeds(unsigned int x, unsigned int y) {
     if (x >= MATRIX_COL_COUNT || y >= MATRIX_LINE_COUNT) {
        return;
    }
  
     // Reset all ports
     DDRB = 0;
     DDRC = 0;
     DDRD = DDRD & 0b00000100; // do not touch PD2 as it has the relay output
     
     PORTB = 0;
     PORTC = 0;
     PORTD = PORTD & 0b00000100;

     DDRB = pgm_read_byte (& (matrix [x] [y] [_DDRB]));
     DDRC = pgm_read_byte (& (matrix [x] [y] [_DDRC]));
     DDRD = DDRD & 0b00000100 | (pgm_read_byte (& (matrix [x] [y] [_DDRD])));

     PORTB = pgm_read_byte (& (matrix [x] [y] [_PORTB]));
     PORTC = pgm_read_byte (& (matrix [x] [y] [_PORTC]));
     PORTD = PORTD & 0b00000100 | (pgm_read_byte (& (matrix [x] [y] [_PORTD])));
 } 

inline void clearLeds() {
    // Reset all ports
    DDRB = 0;
    DDRC = 0;
    DDRD = DDRD & 0b00000100;
    
    PORTB = 0;
    PORTC = 0;
    PORTD = PORTD & 0b00000100;
}


