#ifndef USB_H
#define USB_H
#include <stdint.h>
#ifndef TESTMODE
	#include <avr/pgmspace.h>   /* required by usbdrv.h */
	#include "usbdrv.h"
#else
	typedef uint8_t usbMsgLen_t;
	typedef uint8_t uchar;
	#define USB_CFG_HID_REPORT_DESCRIPTOR_LENGTH 63
	#define PROGMEM
	#include <string.h>
	#define memcpy_P memcpy
	uint8_t usbInterruptIsReady(void);
#endif

// Defines
#define MOUSE_LEFT  0x01
#define MOUSE_RIGHT 0x02
#define MOUSE_MIDDLE 0x04

// structures
struct CharKeyModifier
{
	uint8_t c; //char
	uint8_t k; //keyCode
	uint8_t m; //modifer
};

typedef struct Report_t Report_t;
struct Report_t
{
    uchar   buttonMask;
    char    dx;
    char    dy;
    char    dWheel;
};

extern Report_t reportBuffer;

// Data
extern const char usbHidReportDescriptor[USB_CFG_HID_REPORT_DESCRIPTOR_LENGTH] PROGMEM;
void usbModuleInit(void);
extern usbMsgLen_t usbFunctionSetup(uchar data[8]);

#endif /* USB_H*/
