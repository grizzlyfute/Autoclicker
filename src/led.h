#ifndef LED_H
#define LED_H
#include <stdint.h>

#define ledOff() ledBlink(1, 0, 0xFFFF);
#define ledOn() ledBlink(1, 0xFFFF, 1);

void ledInit(void);
void ledBlink(uint8_t timer, uint16_t tOn, uint16_t tOff);

#endif
