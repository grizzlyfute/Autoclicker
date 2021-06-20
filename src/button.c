#include "button.h"
#include "ports.h"
#include <avr/io.h>
#include <avr/interrupt.h>

void btnInit(void)
{
	DDRB &= ~(1 << PIN_BTN);

	// Enable internal pull up
	MCUCR &= ~(1 << PUD);
	PORTB |= (1 << PIN_BTN);

}

// Return true if pressed more than 5 ms and released
uint8_t btnEvent(uint8_t deltaTimer)
{
	static uint8_t btnTimer = 0xFF;

	// Pressed
	if (~PINB & (1 << PIN_BTN))
	{
		if (btnTimer > deltaTimer) btnTimer -= deltaTimer;
		else btnTimer = 0;
	}
	// Released
	else
	{
		if (btnTimer == 0) btnTimer = 0xFF;
		else btnTimer = 5; // ms
	}

	return btnTimer == 0xFF;
}
