#include <avr/delay.h>
#include <avr/pgmspace.h>

#include "output.h"
#include "config.h"
#include "pollserial.h"

#if defined(ARDUINO) && ARDUINO >= 100
  #include "Arduino.h"
#else
  #include "wiring.h"
#endif

// Lets define some delays
#define delay15()  __asm__("nop\n\t""nop\n\t""nop\n\t""nop\n\t""nop\n\t""nop\n\t""nop\n\t""nop\n\t""nop\n\t""nop\n\t""nop\n\t""nop\n\t""nop\n\t""nop\n\t""nop\n\t")
#define delay14()  __asm__("nop\n\t""nop\n\t""nop\n\t""nop\n\t""nop\n\t""nop\n\t""nop\n\t""nop\n\t""nop\n\t""nop\n\t""nop\n\t""nop\n\t""nop\n\t""nop\n\t")
#define delay13()  __asm__("nop\n\t""nop\n\t""nop\n\t""nop\n\t""nop\n\t""nop\n\t""nop\n\t""nop\n\t""nop\n\t""nop\n\t""nop\n\t""nop\n\t""nop\n\t")
#define delay12()  __asm__("nop\n\t""nop\n\t""nop\n\t""nop\n\t""nop\n\t""nop\n\t""nop\n\t""nop\n\t""nop\n\t""nop\n\t""nop\n\t""nop\n\t")
#define delay11()  __asm__("nop\n\t""nop\n\t""nop\n\t""nop\n\t""nop\n\t""nop\n\t""nop\n\t""nop\n\t""nop\n\t""nop\n\t""nop\n\t")
#define delay10()  __asm__("nop\n\t""nop\n\t""nop\n\t""nop\n\t""nop\n\t""nop\n\t""nop\n\t""nop\n\t""nop\n\t""nop\n\t")
#define delay9()   __asm__("nop\n\t""nop\n\t""nop\n\t""nop\n\t""nop\n\t""nop\n\t""nop\n\t""nop\n\t""nop\n\t")
#define delay8()   __asm__("nop\n\t""nop\n\t""nop\n\t""nop\n\t""nop\n\t""nop\n\t""nop\n\t""nop\n\t")
#define delay7()   __asm__("nop\n\t""nop\n\t""nop\n\t""nop\n\t""nop\n\t""nop\n\t""nop\n\t")
#define delay6()   __asm__("nop\n\t""nop\n\t""nop\n\t""nop\n\t""nop\n\t""nop\n\t")
#define delay5()   __asm__("nop\n\t""nop\n\t""nop\n\t""nop\n\t""nop\n\t")
#define delay4()   __asm__("nop\n\t""nop\n\t""nop\n\t""nop\n\t")
#define delay3()   __asm__("nop\n\t""nop\n\t""nop\n\t")
#define delay2()   __asm__("nop\n\t""nop\n\t")
#define delay1()   __asm__("nop\n\t")

volatile int scanLine;
volatile byte gfx_buffer[GFX_HRES*GFX_VRES/8];

void detectframe() {
  scanLine = 0;
}

void detectline() {
  little_delay(); // This is used to adjust to timing when using SimpleOSD instead of Arduino   

#if ((GFX_SCALE_X == 1) && (GFX_SCALE_Y == 1))
  if (scanLine >= GFX_VSTART && scanLine < (GFX_VSTART + GFX_VRES)) {
    _delay_loop_1(GFX_HALIGN);
    int p = (scanLine - GFX_VSTART) * (GFX_HRES/8);
    for (int i=0; i < GFX_HRES/8; i++) {
      SPDR = gfx_buffer[p++];
      delay2();
    }
    delay9();
    SPDR = 0;
  }
#elif ((GFX_SCALE_X == 1) && (GFX_SCALE_Y == 2))
  if (scanLine >= GFX_VSTART && scanLine < (GFX_VSTART + GFX_VRES * 2)) {
    _delay_loop_1(GFX_HALIGN);
    int p = ((scanLine - GFX_VSTART) >> 1) * (GFX_HRES/8);
    for (int i=0; i < GFX_HRES/8; i++) {
      SPDR = gfx_buffer[p++];
      delay2();
    }
    delay9();
    SPDR = 0;
  }
#elif ((GFX_SCALE_X == 2) && (GFX_SCALE_Y == 1))
  if (scanLine >= GFX_VSTART && scanLine < (GFX_VSTART + GFX_VRES)) {
    _delay_loop_1(GFX_HALIGN);
    int p = (scanLine - GFX_VSTART) * (GFX_HRES/8);
    for (int i=0; i < GFX_HRES/8; i++) {
      SPDR = gfx_buffer[p++];
      _delay_loop_1(6);
    }
    delay9();
    SPDR = 0;
  }
#elif ((GFX_SCALE_X == 2) && (GFX_SCALE_Y == 2))
  if (scanLine >= GFX_VSTART && scanLine < (GFX_VSTART + GFX_VRES * 2)) {
    _delay_loop_1(GFX_HALIGN);
    int p = ((scanLine - GFX_VSTART) >> 1) * (GFX_HRES/8);
    for (int i=0; i < GFX_HRES/8; i++) {
      SPDR = gfx_buffer[p++];
      _delay_loop_1(6);
    }
    delay9();
    SPDR = 0;
  }
#endif

  // Increase line count.
  if (CONTROLLER == 3)
    scanLine = (PIND & 0b00000100) ? scanLine + 1 : 0;
  else
    scanLine++;

  // Let's make sure SPI is not idling high.
  SPDR=0b00000000;

  USART_recieve();
}

void output_setup() {
  // Enable SPI for pixel generation
  // Set SPI;  
  SPCR = 
    (1<<SPE) | //Enable SPI
    (1<<MSTR)| // Set as master
    (0<<SPR1)| // Max speed
    (0<<SPR0)| // Max speed
    (0<<DORD)| // Year, Try to set this to 1 :D (Will mirror the byte)
    (1<<CPOL)| // Doesnt really matter, its the SCK polarity
    (1<<CPHA); //

#if (GFX_SCALE_X == 1)
  SPSR = (1<<SPI2X); // SPI double speed - we want 8 mhz output speed.
#else
  SPSR = (0<<SPI2X); // SPI single speed - we want 4 mhz output speed.
#endif
  SPDR =0b00000000; // IMPORTANT.. The SPI will idle random at low or high in the beginning. If it idles high you will get black screen = bad idea in FPV.
  // It will always idle at the last bit sent, so always be sure the last bit is 0. The main-loop and end of each line will always send 8 zeros - so it should
  // be pretty safe.

  // Disable standard Arduino interrupts used for timer etc. (you can clearly see the text flicker if they are not disabled)
  TIMSK0=0;
  TIMSK1=0;
  TIMSK2=0;

#if (CONTROLLER == 0) // If Arduino is used
  // Init analog comparator to register new line and frame
  ADCSRB = 0b00000001; // Set analog comparator mode
  ACSR = 0b00001011; //Enable Analog comparator interrupt

  // Set interrupt on falling output edge.
  ADCSRA|=(1<<ADSC);

  // Start timer 2 - used to determine if it's a new frame or line. 
  TCCR2B =
    (0<<CS22) | //Prescale 1024
    (0<<CS21) | //Prescale 1024
    (1<<CS20) | //Prescale 1024
    (0<<WGM22); // CTC mode (Clear timer on compare match)

#elif (CONTROLLER == 1) // If SimpleOSD XL/LM1881
  attachInterrupt(0,detectline,FALLING);
  attachInterrupt(1,detectframe,RISING);  
  pinMode(5,OUTPUT);
  digitalWrite(5,HIGH); // Turn on the led

#elif (CONTROLLER == 2) // If SimpleOSD X2/LM1881
  attachInterrupt(0,detectline,FALLING);
  attachInterrupt(1,detectframe,RISING);  
  pinMode(13,INPUT);
  digitalWrite(13,HIGH); // Turn on the led

#elif (CONTROLLER == 3)	// If minimOSD/MAX7456
  attachInterrupt(1,detectline,FALLING);
  pinMode(13,INPUT);
  digitalWrite(13,HIGH); // Turn on the led
#endif
}
