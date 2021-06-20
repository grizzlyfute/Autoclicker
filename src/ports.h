#ifndef PORTS_H
#define PORTS_H

#define DIGISPARK 0

#if (DIGISPARK)

#define PIN_DMINUS 3
#define PIN_DPLUS  4
#define PIN_LED    1
#define PIN_BTN    0
#define PIN_NC     2

// USB Interrupt - PCINT3
#define CUSTOM_USB_INTR_CFG            PCMSK
#define CUSTOM_USB_INTR_CFG_SET        (1 << PCINT3)
#define CUSTOM_USB_INTR_CFG_CLR        0
#define CUSTOM_USB_INTR_ENABLE         GIMSK
#define CUSTOM_USB_INTR_ENABLE_BIT     PCIE
#define CUSTOM_USB_INTR_PENDING        GIFR
#define CUSTOM_USB_INTR_PENDING_BIT    PCIF
#define CUSTOM_USB_INTR_VECTOR         PCINT0_vect

#else // !DIGISPARK

#define PIN_DMINUS 2
#define PIN_DPLUS  1
#define PIN_LED    0
#define PIN_BTN    3
#define PIN_NC     4

// USB Interrupt - INT0
#define CUSTOM_USB_INTR_CFG            MCUCR
#define CUSTOM_USB_INTR_CFG_SET        ((1 << ISC00) | (1 << ISC01))
#define CUSTOM_USB_INTR_CFG_CLR        0
#define CUSTOM_USB_INTR_ENABLE         GIMSK
#define CUSTOM_USB_INTR_ENABLE_BIT     INT0
#define CUSTOM_USB_INTR_PENDING        GIFR
#define CUSTOM_USB_INTR_PENDING_BIT    INTF0
#define CUSTOM_USB_INTR_VECTOR         INT0_vect

#endif // DIGISPARK

#endif /* PORTS_H */
