#ifndef config_h
#define config_h
#include <avr/io.h>

//===========================
// Hardware Arduino or SimpleOSD 16 mhz??
//===========================

// from code version 5, support for 16 mhz SimpleOSD with Arduino bootloader have been added. It uses the same microcontroller etc - but a LM1881 chip is used to detect new lines and frames.

//Arduino = 0, SimpleOSD OPEN = 1, SimpleOSD X2 = 2, minimOSD = 3
#define CONTROLLER 3

//=============================================================================================
// Setup
//=============================================================================================

// graphic display settings
#define GFX_VRES 64
#define GFX_HRES 128
#define GFX_MODE_MAX 3

#define GFX_MODE 2
#define GFX_XSTART 2  // 1..156 HighRes, 1..90 LoRes
#define GFX_YSTART 90 // 22..220 HighRes, 22..175 LoRes

#define GFX_DEMO

//========================================================================
// END SETUP
//========================================================================

// Arduino
#if (CONTROLLER == 0)
  #define little_delay() do {} while (0)

// SimpleOSD XL OPEN 16 mhz
#elif (CONTROLLER==1)
  #define little_delay() _delay_loop_1(13) 

// SimpleOSD X2 16 mhz
#elif (CONTROLLER==2)
  #define little_delay() _delay_loop_1(13)

// minimOSD 16 mhz
#elif (CONTROLLER==3)
  #define little_delay() _delay_loop_1(13)

#endif

#endif
