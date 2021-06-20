#include "button.h"
#include "led.h"
#include "timer.h"
#include "usb.h"

#include <avr/io.h>
#include <avr/wdt.h>
#include <avr/interrupt.h>
#include <avr/pgmspace.h>
//#define DIVIDE_BY_64(val)  (((val) + ((val) > 0 ? 32 : -32)) >> 6)    /* rounding divide */

// 256 * CIRCLE_TIME = circle duration
#define CIRCLE_TIMER_MS 12U
#define LOWFREQCLICK_TIMER_MS 6500U
#define RSTBTNTIMER 5000U

#define ROSETIMER 40U
#define ROSEBRANCHS 4

// 127 * sin (i * 2 * M_PI / 256)
const int8_t sin_pcm[256] PROGMEM =
{
   0,    3,    6,    9,   12,   15,   18,   21,
   24,   27,   30,   33,   36,   39,   42,   45,
   48,   51,   54,   57,   59,   62,   65,   67,
   70,   73,   75,   78,   80,   82,   85,   87,
   89,   91,   94,   96,   98,  100,  102,  103,
   105,  107,  108,  110,  112,  113,  114,  116,
   117,  118,  119,  120,  121,  122,  123,  123,
   124,  125,  125,  126,  126,  126,  126,  126,
   127,  126,  126,  126,  126,  126,  125,  125,
   124,  123,  123,  122,  121,  120,  119,  118,
   117,  116,  114,  113,  112,  110,  108,  107,
   105,  103,  102,  100,   98,   96,   94,   91,
   89,   87,   85,   82,   80,   78,   75,   73,
   70,   67,   65,   62,   59,   57,   54,   51,
   48,   45,   42,   39,   36,   33,   30,   27,
   24,   21,   18,   15,   12,    9,    6,    3,
   0,   -3,   -6,   -9,  -12,  -15,  -18,  -21,
  -24,  -27,  -30,  -33,  -36,  -39,  -42,  -45,
  -48,  -51,  -54,  -57,  -59,  -62,  -65,  -67,
  -70,  -73,  -75,  -78,  -80,  -82,  -85,  -87,
  -89,  -91,  -94,  -96,  -98, -100, -102, -103,
  -105, -107, -108, -110, -112, -113, -114, -116,
  -117, -118, -119, -120, -121, -122, -123, -123,
  -124, -125, -125, -126, -126, -126, -126, -126,
  -127, -126, -126, -126, -126, -126, -125, -125,
  -124, -123, -123, -122, -121, -120, -119, -118,
  -117, -116, -114, -113, -112, -110, -108, -107,
  -105, -103, -102, -100,  -98,  -96,  -94,  -91,
  -89,  -87,  -85,  -82,   -80,  -78,  -75,  -73,
  -70,  -67,  -65,  -62,   -59,  -57,  -54,  -51,
  -48,  -45,  -42,  -39,   -36,  -33,  -30,  -27,
  -24,  -21,  -18,  -15,   -12,   -9,   -6,   -3,
};

int main(void)
{
	// Fuse: see makefile
	// FuseExtended : 0xb1111111 = 0xFF
	// FuseHigh: ( BOD 2.7V clock 8 divider off ) 0b11011101 = 0xDD
	// FuseLow (CKSEL = PLL 16Mhz): 0b11100001 = 0xE1

	uint8_t mode = 0;
	uint8_t delta = 0;
	uint8_t oldTimer = 0;
	uint8_t step = 0;
	int8_t x = 0,y = 127;
	uint16_t stepTimer = 0;
	uint16_t rstBtnTimer = 0;

	cli();

	// Warning: USB module use INT0
	// USB configuration
	usbModuleInit();

	//led setup
	ledInit();

	// Button setup
	btnInit();

	// Stoping useless module
	PRR = (1 << PRTIM1) | // Shut down timer 1
		  (0 << PRTIM0) | // Power on timer 0
		  (1 << PRUSI)  | // Shut down usart
		  (1 << PRADC)  | // Shut down Analog Digital Converter
		  0x00;

	// System timer
	timerInit();

	// Set PIN NC to low. Used as btn voltage ref
	DDRB |= (1 << PIN_NC);
	PORTB &= ~(1 << PIN_NC);

	// Shutdown useless
	PRR = // Power Reduction Register
		(1 << PRTIM1) | // disable timer1
		(0 << PRTIM0) | // enable timer0
		(1 << PRUSI)  | // disable Universal Serial Interface
		(1 << PRADC)  ; // disable Analog Digital Converter

	// Watchdog configuration
	wdt_enable(WDTO_1S);

	sei();

	// Main loog
	for(;;)
	{
		// Watchdog
		wdt_reset();

		// Timer management
		delta = timerGet() - oldTimer;
		oldTimer += delta;
		if (stepTimer > delta) stepTimer -= delta;
		else stepTimer = 0;
		if (rstBtnTimer > delta) rstBtnTimer -= delta;
		else (rstBtnTimer = 0);

		// Usb management
        usbPoll();
		if (usbInterruptIsReady())
		{
			reportBuffer.buttonMask = 0;
			reportBuffer.dx = 0;
			reportBuffer.dy = 0;
			reportBuffer.dWheel = 0;

			// Round
			if (mode == 1)
			{
				if (stepTimer <= 0)
				{
					// x = cos(t)
					// y = sin(t)
					reportBuffer.dx = x;
					reportBuffer.dy = y;
					x = pgm_read_byte(&sin_pcm[(step - 64) & 0XFF]);
					y = pgm_read_byte(&sin_pcm[step]);
					reportBuffer.dx = x - reportBuffer.dx;
					reportBuffer.dy = y - reportBuffer.dy;
					step++;
					stepTimer = CIRCLE_TIMER_MS;
				}
			}
			// Left click, low freq
			else if (mode == 2)
			{
				if (step == 0)
				{
					reportBuffer.buttonMask = MOUSE_LEFT;
					step = 1;
				}
				else if (step == 1)
				{
					stepTimer = LOWFREQCLICK_TIMER_MS;
					step = 2;
				}
				else if (step == 2)
				{
					if (stepTimer <= 0) step = 0;
				}
				else step = 0;
			}
			// Left click, high freq
			else if (mode == 3)
			{
				reportBuffer.buttonMask = step ? MOUSE_LEFT : 0;
				step = !step;
			}
			// Drawing a rose
			else if (mode == 4)
			{
				if (stepTimer <= 0)
				{
					// x = cos(k*t)*sin(t)
					// y = cos(k*t)*cos(t)
					reportBuffer.dx = x;
					reportBuffer.dy = y;
					x = pgm_read_byte(&sin_pcm[(ROSEBRANCHS * (step - 64)) & 0XFF]);
					y = (x * (int8_t)pgm_read_byte(&sin_pcm[(step - 64) & 0xFF])) / 127;
					x = (x * (int8_t)pgm_read_byte(&sin_pcm[(step) & 0xFF])) / 127;
					reportBuffer.dx = x - reportBuffer.dx;
					reportBuffer.dy = y - reportBuffer.dy;
					step++;
					if (step > 0) stepTimer = ROSETIMER;
					else stepTimer = 256*ROSETIMER;
				}
				if (step > 0) reportBuffer.buttonMask = MOUSE_LEFT;
			}
			else
			{
				// Does nothing
				mode = 0;
				step = 0;
			}

			usbSetInterrupt((void *)&reportBuffer, sizeof(reportBuffer));
		}

		// Led management
		if (mode == 1)
		{
			ledBlink(delta, 1000, 1000);
		}
		else if (mode == 2)
		{
			ledBlink(delta, 200, 1000);
		}
		else if (mode == 3)
		{
			ledBlink(delta, 200, 200);
		}
		else if (mode == 4)
		{
			ledBlink(delta, 1000, 200);
		}
		else
		{
			ledBlink(delta, 500, 3000);
		}

		// Button management
		if (btnEvent(delta))
		{
			if (rstBtnTimer <= 0 && mode > 0)
				mode = 0;
			else
				mode = (mode + 1) % 5;
			rstBtnTimer = RSTBTNTIMER;
			stepTimer = 0;
			step = 0;
			x = 0;
			y = 0;
		}
	}

	return 1;
}
