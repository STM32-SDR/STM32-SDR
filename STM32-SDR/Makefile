#
#       !!!! Do NOT edit this makefile with an editor which replace tabs by spaces !!!!   
#
##############################################################################################
#
# On command line:
#
# make all = Create project
#
# make clean = Clean project files.
#
# To rebuild project do "make clean" and "make all".
#
# Included originally in the yagarto projects. Original Author : Michael Fischer
# Modified to suit our purposes by Hussam Al-Hertani
# Use at your own risk!!!!!
#
#	Modified by KB4FXC, 2012/08/23, to use static libraries rather
#	than library object files. 
#
##############################################################################################
# Start of default section
#
TRGT = arm-none-eabi-
#TRGT = /usr/local/arm-toolchain/bin/arm-none-eabi-
CC   = $(TRGT)gcc
CP   = $(TRGT)objcopy
AS   = $(TRGT)gcc -x assembler-with-cpp
HEX  = $(CP) -O ihex
BIN  = $(CP) -O binary -S
MCU  = cortex-m4
 
# List all default C defines here, like -D_DEBUG=1
DDEFS = -DSTM32F4XX -DUSE_STDPERIPH_DRIVER -DARM_MATH_CM4 -D__FPU_PRESENT

# List all default ASM defines here, like -D_DEBUG=1
DADEFS =
 
# List all default directories to look for include files here
DINCDIR =
 
# List the default directory to look for the libraries here
DLIBDIR =
 
# List all default libraries here
DLIBS =
 
#
# End of default section
##############################################################################################
 
##############################################################################################
# Start of user section
#
 
#
# Define project name and Ram/Flash mode here
PROJECT        = STM32-SDR
RUN_FROM_FLASH = 1
 
# List all user C define here, like -D_DEBUG=1
UDEFS = 
 
# Define ASM defines here
UADEFS =
 
# List C source files here
# Note: Use \ (or \\ if being processed by a tool) for paths because
#       Linux tools handle either, but Windows tools (some) only handle \.
LIBSDIR      = .\externals\SM32F4-Discovery
CORELIBDIR   = $(LIBSDIR)\CMSIS-Includes
DEVDIR       = $(LIBSDIR)\STM32F4xx-Includes
STMSPDDIR    = $(LIBSDIR)\STM32F4xx-StdPeriph-Includes
USBINCDIR    = $(LIBSDIR)\USB\inc
LINKER       = .\linker

# Build all .c files in the src/ folder.
# wildcard needs unix forward-slashes, but build tools need wisdows backslashes, 
# so give wildcard what it likes, and then convert the results using subst function.
SRC_TMP = $(wildcard ./src/*.c) $(wildcard ./externals/SM32F4-Discovery/USB/src/*.c)
SRC = $(subst /,\,$(SRC_TMP))

# List ASM source files here
ASRC = .\startup\startup_stm32f4xx.s
 
# List all user directories here
UINCDIR = $(DEVDIR) $(CORELIBDIR) $(STMSPDDIR)  $(USBINCDIR) .\inc     
     
# List the user directory to look for the libraries here
ULIBDIR = .\externals\libs
 
# List all user libraries here
ULIBS = -lstm32f4xx_peripherals -lstm32f4xx_math -lm
 
# Define optimisation level here
OPT = -Os

# Folder for build intermediates:
OBJDIR = obj

# Required directories:
DIRS = $(OBJDIR) $(OBJDIR)\src $(OBJDIR)\externals $(OBJDIR)\externals\SM32F4-Discovery $(OBJDIR)\externals\SM32F4-Discovery\USB $(OBJDIR)\externals\SM32F4-Discovery\USB\src $(OBJDIR)\startup .dep

#
# End of user defines
##############################################################################################
#
# Define linker script file here
#
ifeq ($(RUN_FROM_FLASH), 0)
LDSCRIPT = $(LINKER)/stm32f407vg.ld
FULL_PRJ = $(PROJECT)_ram
else
LDSCRIPT = ./linker/stm32f407vg.ld
FULL_PRJ = $(PROJECT)_rom
endif
 
INCDIR  = $(patsubst %,-I%,$(DINCDIR) $(UINCDIR))
LIBDIR  = $(patsubst %,-L%,$(DLIBDIR) $(ULIBDIR))
 
ifeq ($(RUN_FROM_FLASH), 0)
DEFS    = $(DDEFS) $(UDEFS) -DRUN_FROM_FLASH=0 -DVECT_TAB_SRAM
else
DEFS    = $(DDEFS) $(UDEFS) -DRUN_FROM_FLASH=1
endif
 
ADEFS   = $(DADEFS) $(UADEFS)
OBJS    = $(patsubst %,$(OBJDIR)\\%,$(ASRC:.s=.o)) $(patsubst %,$(OBJDIR)\\%,$(SRC:.c=.o))

LIBS    = $(DLIBS) $(ULIBS)
MCFLAGS = -mcpu=$(MCU) -mfpu=fpv4-sp-d16 -mfloat-abi=hard
 
ASFLAGS = $(MCFLAGS) -g -gdwarf-2 -mthumb  -Wa,-amhls=$(OBJDIR)/$(<:.s=.lst) $(ADEFS)
CPFLAGS = $(MCFLAGS) $(OPT) -std=c99 -gdwarf-2 -mthumb   -fomit-frame-pointer -Wall -Wstrict-prototypes -fverbose-asm -Wa,-ahlms=$(OBJDIR)/$(<:.c=.lst) $(DEFS)
LDFLAGS = $(MCFLAGS) -mthumb -nostartfiles -T$(LDSCRIPT) -Wl,-Map=$(FULL_PRJ).map,--cref,--no-warn-mismatch $(LIBDIR)
 
# Generate dependency information
CPFLAGS += -MD -MP -MF .dep/$(@F).d
 
#
# makefile rules
#

all: dirs $(OBJS) $(FULL_PRJ).elf  $(FULL_PRJ).hex $(FULL_PRJ).bin
ifeq ($(RUN_FROM_FLASH), 0)
	$(TRGT)size $(PROJECT)_ram.elf
else
	$(TRGT)size $(PROJECT)_rom.elf
endif
 
dirs: $(DIRS)
$(DIRS):
	mkdir $@

$(OBJDIR)\\%.o: %.c
	$(CC) -c $(CPFLAGS) -I . $(INCDIR) $< -o $@

$(OBJDIR)\\%.o: %.s
	$(AS) -c $(ASFLAGS) $< -o $@

%elf: $(OBJS)
	$(CC) $(OBJS) $(LDFLAGS) $(LIBS) -o $@

%hex: %elf
	$(HEX) $< $@
	
%bin: %elf
	$(BIN)  $< $@
	
unixclean:
	rm -f $(OBJS)
	rm -f $(OBJS:.o=.lst)
	rm -f $(FULL_PRJ).elf
	rm -f $(FULL_PRJ).map
	rm -f $(FULL_PRJ).hex
	rm -f $(FULL_PRJ).bin
	rm -f $(SRC:.c=.c.bak)
	rm -f $(ASRC:.s=.s.bak)
	rm -rf .dep
	mkdir .dep
	
clean:
	del $(OBJS)
	del $(OBJS:.o=.lst)
	del $(FULL_PRJ).elf
	del $(FULL_PRJ).map
	del $(FULL_PRJ).hex
	del $(FULL_PRJ).bin
	del $(SRC:.c=.c.bak)
	del $(ASRC:.s=.s.bak)
	rmdir .dep /S /Q
	mkdir .dep


# Used for debugging the makefile variables.
dump:
	@echo Dumping out some build variables:
	@echo SRC:  $(SRC)
	@echo 
	@echo ASRC: $(ASRC)
	@echo
	@echo OBJS: $(OBJS)
	@echo
	@echo Your GCC version:
	$(CC) --version
	@echo
	@echo Your make version:
	make --version
 


# *** EOF ***
