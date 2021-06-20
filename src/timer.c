#include "timer.h"
#include <avr/io.h>
#include <avr/interrupt.h>

static uint8_t timerTick;

void timerInit(void)
{
	// period = 1 / (freq / (prescal * OCRA)) = prescal * OCRA / freq
	// => OCRA = freq * period / prescal
	// Interrupt on counter0 every 1 ms with 16Mhz for frequency (default fuse)
	TCCR0A = (1 << WGM01); //Clear timer on compare match (CTC)
	TCCR0B = (1 << CS02) | (0 << CS01) | (1 << CS00); //use 1024 as prescalar
	OCR0A = 16;
	TIMSK |= (1 << OCIE0A); // Timer/Counter0 Output Compare Match A Interrupt Enable
	timerTick = 0;
}

uint8_t timerGet()
{
	uint8_t ret = 0;
	TIMSK &= ~(1 << OCIE0A);
	ret = timerTick;
	TIMSK |= (1 << OCIE0A);
	return ret;
}

ISR(TIMER0_COMPA_vect)
{
	timerTick++;
}
