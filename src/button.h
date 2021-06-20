#ifndef BUTTON_H
#define BUTTON_H

#include <stdint.h>

// BTN is connected beetween pin btn (internal pull up) and pin nc (0v)
// PCINT0 vect is used by v-usb in case of digispark circuit
// Do soft button management
void btnInit(void);
uint8_t btnEvent(uint8_t timer);

#endif /* BUTTON_H */
