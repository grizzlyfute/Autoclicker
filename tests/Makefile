#make to run all test
#make SRC=testSekeleton.c to run testSekeleton.c

#PROGRAMS
CC = gcc
RM = rm -f
EXT = c

#DIRECTORIES
SRCDIR = .
RSRCDIR= ../src
INCDIR = $(RSRCDIR)
BINDIR = .

#PARSE SOURCES FILES
#use notdir to remove directory (substitution : deux methodes)

ifeq ($(SRC),)
SRC = $(wildcard $(SRCDIR)/*.$(EXT))
OUT  = $(filter %.out,$(SRC:$(SRCDIR)/%.$(EXT)=$(BINDIR)/%.out))
else
OUT  = $(filter %.out,$(SRC:%.c=$(BINDIR)/%.out))
endif

#FLAGS
LDFLAGS =
CFLAGS = -Wall -O0 -Wextra $(patsubst %,-I%, $(INCDIR)) -pipe -Wshadow -g -DTESTMODE
CFLAGS+= -fdiagnostics-color -fno-diagnostics-show-caret

#.PHONY: des regles et non des noms de fichiers
.PHONY: clean all mrproper
.SILENT: $(q)


#COMPILATIONS RULES
all: .depend $(OUT)
	$(foreach out,$(OUT),$(out);)

clean:
		$(RM) $(OUT)
		$(RM) .depend
#$(BINDIR)/%.out: $(SRCDIR)/%.$(EXT)
#		#@echo CC $@
#		$(CC) -o $@  $< $(CFLAGS)

# auto generated rule...
.depend:
	$(CC) -MM -I$(INCDIR) $(SRC) $(CFLAGS) > $@
	sed -i 's/\(.*\)\.o:/\1.out:/g' $@
	sed -i 's/$(subst /,\/,$(INCDIR))\/\([a-zA-Z0-9]\+\)\.h/$(subst /,\/,$(INCDIR))\/\1.h $(subst /,\/,$(RSRCDIR))\/\1.$(EXT)/g' $@
	sed -i 's/\(.*[^\\]$$\)/\1\n\t$(CC) -o $$@  $$^ $$(CFLAGS)/g' $@

-include .depend
