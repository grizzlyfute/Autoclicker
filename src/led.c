#include "led.h"
#include "ports.h"
#include <avr/io.h>

void ledInit(void)
{
	DDRB |= (1 << PIN_LED);
	PORTB &= ~(1 << PIN_LED);
}

void ledBlink(uint8_t timer, uint16_t tOn, uint16_t tOff)
{
	static uint16_t txx = 0;
	txx += timer;
	if (txx < tOn) PORTB |= (1 << PIN_LED);
	else PORTB &= ~(1 << PIN_LED);
	if (txx >= tOn + tOff) txx = 0;
}
