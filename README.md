HoTT_OSD
========

This is a little project to get Graupner HoTT telemetry data from your RC gear on ground and mixed into the video signal.
The advantage is not to have more than the needed electronic on the model and you can also use standard graupner sensors for fpv.

It is tested on modified minimOSD hardware using a MAX7456. You can also use a LM1881 a sync separator. 

The minimOSD needs the following modifications:
 * wire GND from UART port to (-) on video side
 * wire +5V from UART port to the coil on video side to power the MAX7456
 * wire HSYNC from MAX7456 to ATMEGA328 Pin1 (INT1)
 * wire a diode (1N4148) and a resistor (220Ω) in series from ATMEGA328 Pin15 (MOSI) to VideoIN
