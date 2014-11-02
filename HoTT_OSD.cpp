/*

Copyright (c) 2014.  All rights reserved.
An Open Source Arduino based OSD project.

Program  : HoTT_OSD (based on ArduCAM-OSD (MinimOSD [and variants]) and SimpleOSD

This program is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program. If not, see <http://www.gnu.org/licenses/>

*/

/* **********************************************/
/* ***************** INCLUDES *******************/

// AVR Includes
#include <stdlib.h>
#include <avr/io.h>

// Get the common arduino functions
#if defined(ARDUINO) && ARDUINO >= 100
  #include "Arduino.h"
#else
  #include "wiring.h"
#endif

#include "config.h"
#include "output.h"
#include "pollserial.h"
#include "Max7456.h"

#include <avr/pgmspace.h>

/* *************************************************/
/* ***************** DEFINITIONS *******************/

pollserial pserial;
extern volatile int scanLine;
extern unsigned char gfx_buffer[];

#define BLACK	0
#define WHITE	1
#define INVERT	2

/* *********************************************** */
/* ******** functions used in main loop() ******** */

void delay_ms(int time) {
  TIFR0 = (1<<TOV0);
  while (time-- > 0) {
    while ((TIFR0 & (1<<TOV0)) == 0) {}
    TIFR0 = (1<<TOV0);
  }
}

static void inline sp(unsigned char x, unsigned char y, char c) {
  if (c==WHITE)
    gfx_buffer[(x/8) + (y*GFX_HRES/8)] |= 0x80 >> (x&7);
  else if (c==BLACK)
    gfx_buffer[(x/8) + (y*GFX_HRES/8)] &= ~0x80 >> (x&7);
  else
    gfx_buffer[(x/8) + (y*GFX_HRES/8)] ^= 0x80 >> (x&7);
}

void GetHoTT() {
  // send monitor request
  pserial.write(0x7f);
  delay_ms(4);
  pserial.write(0xff);
  delay_ms(8);

  // decode answer
  int state = 0;
  int timeout = 100;
  int x = 0;
  int y = 0;
  int p = 0;

  while (timeout > 0) {
    int c = pserial.read();
    
    if (c < 0) {
      delay_ms(1);
      timeout--;
    } else {
      switch (state) {
      case 0:
        if (c == 0x7b) {
          // first byte of header is ok
          state = 1;
          timeout = 10;
        }
        break;
      case 1:
        if (c == 0xff) {
          // second byte of header is ok
          state = 2;
          timeout = 10;
        } else {
          state = 0;
        }
        break;
      case 2:
        //data goes to image buffer
        sp(x, y + 0, (c & 0x01) ? WHITE : BLACK);
        sp(x, y + 1, (c & 0x02) ? WHITE : BLACK);
        sp(x, y + 2, (c & 0x04) ? WHITE : BLACK);
        sp(x, y + 3, (c & 0x08) ? WHITE : BLACK);
        sp(x, y + 4, (c & 0x10) ? WHITE : BLACK);
        sp(x, y + 5, (c & 0x20) ? WHITE : BLACK);
        sp(x, y + 6, (c & 0x40) ? WHITE : BLACK);
        sp(x, y + 7, (c & 0x80) ? WHITE : BLACK);

        x = (x+1) % 128;
        if (x==0)
          y +=8;
        
        p++;
        state = (p < 1024) ? 2 : 3;
        timeout = 10;
        break;
      case 3:
        break;
      }
    }
  }
}

/* **********************************************/
/* ***************** SETUP() *******************/

void setup() {
  // Set pin-modes
  pinMode(10,OUTPUT);
  pinMode(11,OUTPUT); 
  pinMode(12,INPUT);
  pinMode(13,OUTPUT);
  pinMode(0,INPUT);  
  pinMode(1,INPUT);
  pinMode(2,INPUT);
  pinMode(3,INPUT);
  pinMode(4,INPUT);  
  pinMode(5,INPUT);
  pinMode(6,INPUT); 
  pinMode(7,INPUT);
  pinMode(8,INPUT);

  // init controller specific
#if (CONTROLLER == 3)
  digitalWrite(MAX7456_SELECT,  HIGH);
  delay(500);
  // Prepare MAX7456 for displaying
  OSD osd;
  osd.setMode(1);
  osd.init();
  delay(100);
  digitalWrite(MAX7456_SELECT,  HIGH);
  delay(500);
  osd.clear();
  osd.setPanel(1,1);
  osd.openPanel();
  osd.write('H');
  osd.write('o');
  osd.write('T');
  osd.write('T');
  osd.write('-');
  osd.write('O');
  osd.write('S');
  osd.write('D');
  osd.write('|');
  osd.closePanel();
  delay(500);
  digitalWrite(MAX7456_SELECT,  HIGH);
#endif

  // init serial 115200Bd, 9Bit
  pserial.begin(115200);
  UCSR0B |= (1<<UCSZ02) | (1<<TXB80);

  // timer 0 setup for delays
  TCCR0A = 0;
  TCCR0B = (0 << CS02) | (1 << CS01) | (1 << CS00);

  output_setup();
}

/* ***********************************************/
/* ***************** MAIN LOOP *******************/

void loop() {

#ifdef GFX_DEMO
  int mode = GFX_MODE;
  int x = GFX_XSTART;
  int y = GFX_YSTART;
  int dx = 1;
  int dy = 1;
#endif

  for (int x=0;x<GFX_HRES;x++) {
    sp(x,0,INVERT);
    sp(x,GFX_VRES-1,INVERT);
  }

  for (int y=0;y<GFX_VRES;y++) {
    sp(0,y,INVERT);
    sp(GFX_HRES-1,y,INVERT);
    sp(y,y,INVERT);
  }

  while (1) {
    GetHoTT();
    delay_ms(40);

#ifdef GFX_DEMO
    while (scanLine != 5);
    x = x + dx;
    if (x > 70) {
      dx = -1;
      mode++;
    } else if (x < 2) {
      dx = 1;
      mode++;
    }

    y = y + dy;
    if (y > 150) {
      dy = -1;
      mode++;
    } else if (y < 30) {
      dy = 1;
      mode++;
    }

    mode %= 4;
    output_mode(mode,x,y);
#endif

  }
}

