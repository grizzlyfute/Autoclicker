# #####################################################################
# Important
# see /usr/share/arduino/harware/arduino/board.txt for information
# #####################################################################

#make upload to upload
#make to compile
#make size for your information

SRCDIR = src
INCDIR = src
OBJDIR = obj
BINDIR = bin
LIBS = vusb-20121206/usbdrv vusb-20121206/libs-device

#PORT = /dev/ttyACM0 #arduino / leonardo
PORT = usb
TARGET = $(BINDIR)/autoClicker
MCU = attiny85

#cpu speed
F_CPU = 16000000
#intel hex format
FORMAT = ihex
#UPLOAD_RATE = 115200 #arduino
#UPLOAD_RATE = 57600 #leonardo
UPLOAD_RATE = 8192 #usbasp

# Name of this Makefile (used for "make depend").
MAKEFILE = Makefile

# Debugging format.
# Native formats for AVR-GCC's -g are stabs [default], or dwarf-2.
# AVR (extended) COFF requires stabs, plus an avr-objcopy run.
DEBUG = stabs

# Optimization options : from 0 to 3 with s and fast
OPT = s

# Place -D or -U options here
DEFS = -DF_CPU=$(F_CPU)

# Compiler flag to set the C Standard level.
# c89   - "ANSI" C
# gnu89 - c89 plus GCC extensions
# c99   - ISO C99 standard (not yet fully implemented)
# gnu99 - c99 plus GCC extensions
CSTANDARD = -std=c99
CDEBUG = -g$(DEBUG)
CWARN = -Wall -Wstrict-prototypes -Wextra
CTUNING = -funsigned-char -funsigned-bitfields -fpack-struct -fshort-enums -fdata-sections -ffunction-sections
CEXTRA= -mrelax -fdiagnostics-color -fno-diagnostics-show-caret
#CEXTRA += -Wa,-adhlns=$(<:.c=.lst)

CFLAGS =   $(CDEBUG) $(DEFS) $(INCFLAGS) -O$(OPT) $(CWARN) $(CEXTRA) $(CTUNING) $(CSTANDARD)
CXXFLAGS = $(CDEBUG) $(DEFS) $(INCFLAGS) -O$(OPT) $(CWARN) $(CEXTRA) $(CTUNING)
#ASFLAGS = -Wa,-adhlns=$(<:.S=.lst),-gstabs $(INCFLAGS) $(DEFS) #FIXME
ASFLAGS = $(INCFLAGS) $(DEFS)
LDFLAGS = -Wl,-gc-sections

# Programming support using avrdude. Settings and variables.
#AVRDUDE_PROGRAMMER = stk500v1 #arduino (speed 115200)
#AVRDUDE_PROGRAMMER = avr109 #leonardo (speed 57600)
AVRDUDE_PROGRAMMER = usbasp #attiny / usb (speed 8195)
AVRDUDE_PORT = $(PORT)
AVRDUDE_WRITE_FLASH = -U flash:w:$(TARGET).hex -U eeprom:w:$(TARGET).eep
AVRDUDE_WRITE_FLASH += -U lfuse:w:0xe1:m -U hfuse:w:0xdd:m -U efuse:w:0xff:m #datasheet p148
AVRDUDE_FLAGS = -F -p $(MCU) -P $(AVRDUDE_PORT) -c $(AVRDUDE_PROGRAMMER) \
  -b $(UPLOAD_RATE)

# Program settings
CC = avr-gcc
CXX = avr-g++
OBJCOPY = avr-objcopy
OBJDUMP = avr-objdump
SIZE = avr-size
NM = avr-nm
AVRDUDE = avrdude
REMOVE = rm -fv
MV = mv -fv
SED = sed

###########################################################
#END of settings
###########################################################

INCFLAGS = -I$(INCDIR) $(foreach lib,$(LIBS),-I$(lib))

SRC = $(wildcard $(SRCDIR)/*.c) $(wildcard $(SRCDIR)/*.cpp) $(wildcard $(SRCDIR)/*.S)
LIBSRC = $(foreach lib,$(LIBS),$(wildcard $(lib)/*.c) $(wildcard $(lib)/*.cpp) $(wildcard $(lib)/*.S))

# Define all object files.
OBJ = $(filter %.o,$(SRC:$(SRCDIR)/%.c=$(OBJDIR)/%.o) $(SRC:$(SRCDIR)/%.cpp=$(OBJDIR)/%.o) $(SRC:$(SRCDIR)/%.S=$(OBJDIR)/%.o))
OBJ += $(filter %.o,$(foreach a,$(LIBSRC),$(OBJDIR)/$(basename $(a)).o))

# Define all listing files.
LST = $(filter %.lst, $(SRC:.S=.lst) $(SRC:.cpp=.lst) $(SRC:.c=.lst))

# Combine all necessary flags and optional flags.
# Add target processor to flags.
ALL_CFLAGS = -mmcu=$(MCU) -I. $(CFLAGS)
ALL_CXXFLAGS = -mmcu=$(MCU) -I. $(CXXFLAGS)
ALL_ASFLAGS = -mmcu=$(MCU) -I. $(ASFLAGS) -x assembler-with-cpp

# Default target.
all: .depend build size

build: elf hex eep .depend

elf: $(TARGET).elf
hex: $(TARGET).hex
eep: $(TARGET).eep
lss: $(TARGET).lss
sym: $(TARGET).sym

# Program the device.
upload: $(TARGET).hex $(TARGET).eep $(TARGET).elf
	$(AVRDUDE) $(AVRDUDE_FLAGS) $(AVRDUDE_WRITE_FLASH)


# Convert ELF to COFF for use in debugging / simulating in AVR Studio or VMLAB.
COFFCONVERT=$(OBJCOPY) --debugging \
--change-section-address .data-0x800000 \
--change-section-address .bss-0x800000 \
--change-section-address .noinit-0x800000 \
--change-section-address .eeprom-0x810000


coff: $(TARGET).elf
	$(COFFCONVERT) -O coff-avr $(TARGET).elf $(TARGET).cof


extcoff: $(TARGET).elf
	$(COFFCONVERT) -O coff-ext-avr $(TARGET).elf $(TARGET).cof


.SUFFIXES: .elf .hex .eep .lss .sym .fuse .lock
.SILENT: $(v)

.elf.hex:
	$(OBJCOPY) -O $(FORMAT) -R .eeprom $< $@

.elf.eep:
	$(OBJCOPY) -j .eeprom --set-section-flags=.eeprom="alloc,load" \
	--change-section-lma .eeprom=0 -O $(FORMAT) $< $@

# Create extended listing file from ELF output file.
.elf.lss:
	$(OBJDUMP) -h -S $< > $@

# Create a symbol table from ELF output file.
.elf.sym:
	$(NM) -n $< > $@


# Link: create ELF output file from object files.
$(TARGET).elf: $(OBJ)
	$(CC) $(ALL_CFLAGS) $^ --output $@ $(LDFLAGS)
	@echo LK $@

#use -S to create assembly
# Compile: create object files from C++ source files.
$(OBJDIR)/%.o: $(SRCDIR)/%.cpp
	$(CXX) -c $(ALL_CXXFLAGS) $< -o $@
	@echo CXX $@

$(OBJDIR)/%.o: %.cpp
	$(CXX) -c $(ALL_CXXFLAGS) $< -o $@
	@echo [LIB] CXX $@

# Compile: create object files from C source files.
$(OBJDIR)/%.o: $(SRCDIR)/%.c
	$(CC) -c $(ALL_CFLAGS) $< -o $@
	@echo CC $@

$(OBJDIR)/%.o: %.c
	$(CC) -c $(ALL_CFLAGS) $< -o $@
	@echo [LIB] CC $@

# Assemble: create object files from assembler source files.
$(OBJDIR)/%.o: $(SRCDIR)/%.S
	$(CC) -c $(ALL_ASFLAGS) $< -o $@
	@echo AS $@

$(OBJDIR)/%.o: %.S
	$(CC) -c $(ALL_ASFLAGS) $< -o $@
	@echo [LIB] AS $@

#other rules
size: $(TARGET).elf
	@$(SIZE) --mcu=$(MCU) --format=avr $<

# Target: clean project.
clean:
	$(REMOVE) $(TARGET).hex $(TARGET).eep $(TARGET).cof $(TARGET).elf \
	$(TARGET).map $(TARGET).sym $(TARGET).lss \
	$(OBJ) $(LST)
	$(REMOVE) .depend

.depend:
	$(CC) -MM $(INCFLAGS) $(SRC) $(LIBSRC) $(ALL_CFLAGS) > $@
	$(SED) -i 's/\(.*\.o:\)/$(OBJDIR)\/\1/g' $@

# Including dependencies
-include .depend

.PHONY:	all build elf hex eep lss sym program coff extcoff clean size
