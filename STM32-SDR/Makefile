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
# To rebuild project for "normal" flash programming do "make clean" and "make all".
# To rebuild project for bootloading do "make clean" and "make bootloadable" 
#
# Included originally in the yagarto projects. Original Author : Michael Fischer
# Modified to suit our purposes by Hussam Al-Hertani, and then by Brian Fraser
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
PROJECT_BL     = STM-SDR-BootloadImage
bootloadable: PROJECT = $(PROJECT_BL)

RUN_FROM_FLASH = 1

# Define the base address for the software; 0x80080000 for bootloader.
BASE_ADDRESS = 0x08000000
bootloadable: BASE_ADDRESS = 0x08008000	

 
# List all user C define here, like -D_DEBUG=1
UDEFS = -DBASE_ADDRESS=$(BASE_ADDRESS)
 
# Define ASM defines here
UADEFS =
 
# Note: Use \ (or \\ if being processed by a tool) for paths because
#       Linux tools handle either, but Windows tools (some) only handle \.
LINKER       = .\linker


# Find all .c & .h files
# wildcard needs unix forward-slashes, but build tools need wisdows backslashes, 
# so give wildcard what it likes, and then convert the results using subst function.
#
# Make does not offer a recursive wildcard function, so here's one:
# Posted by "LightStruk" to StackOverflow: http://stackoverflow.com/questions/4036191/sources-from-subdirectories-in-makefile
rwildcard=$(wildcard $1$2) $(foreach d,$(wildcard $1*),$(call rwildcard,$d/,$2))
#
# Recursively find all .c files and their folders 
SRC_TMP := $(call rwildcard,./,*.c)
SRC := $(subst /,\,$(SRC_TMP))
# ..find folders (sort to remove duplicates)
C_DIRS := $(sort $(dir $(SRC)))
# ..(strip any trailing \ at end of each path)
C_DIRS := $(C_DIRS:\=)     
# 
# Recursively find all .h files and their folders 
HEADERS_TMP := $(call rwildcard,./,*.h)
HEADERS := $(subst /,\,$(HEADERS_TMP))
# ..find folders (sort to remove duplicates)
H_DIRS := $(sort $(dir $(HEADERS)))
# ..(strip any trailing \ at end of each path)
H_DIRS := $(H_DIRS:\=)     





# List ASM source files here
ASRC := .\startup\startup_stm32f4xx.s
 
# List all user include directories here  
UINCDIR := $(H_DIRS)     
     
# List the user directory to look for the pre-compiled libraries here
ULIBDIR := .\externals\libs
 
# List all user libraries here
#ULIBS := -lstm32f4xx_peripherals -lstm32f4xx_math -lm
ULIBS := -lstm32f4xx_peripherals -larm_cortexM4l_math -lm
 
# Define optimisation level here 
# (-O0 useful for debugging because variables not optimized out; however, seems to interfere with DMA ISR)
# TODO: Why does -O0 cause the DMA ISR DMA1_Stream0_IRQHandler() to not function?
#   Note that in Init_DMA.c::Audio_DMA_Init(), adding a big delay (Delay(10000000);) after each of 
#   DMA_Cmd(DMA1_Stream0, ENABLE); and DMA_Cmd(DMA1_Stream5, ENABLE); with -O0 causes the IRQ to be triggered
#   continually; without these delays, the IRQ is never triggered.
OPT = -Os

# Folder for build intermediates:
OBJDIR := obj

# Required directories to create:
# - Add the OBJDIR prefix to put the .o files in separate output directory.
DIRS_TMP := $(foreach srcDir,$(C_DIRS),$(OBJDIR)\$(srcDir))  $(OBJDIR)\startup .dep
DIRS := $(subst \.\,\,$(DIRS_TMP))
  

#
# End of user defines
##############################################################################################
#
# Define linker script file here
#
# Choose the script based on if we need normal flash programming, or bootloader support.
LDSCRIPT = $(LINKER)/stm32f407vg.ld
bootloadable: LDSCRIPT = $(LINKER)/stm32f407vg-bootloadable.ld

ifeq ($(RUN_FROM_FLASH), 0)
FULL_PRJ = $(PROJECT)_ram
else
FULL_PRJ = $(PROJECT)_rom
endif
 
INCDIR  := $(patsubst %,-I%,$(DINCDIR) $(UINCDIR))
LIBDIR  := $(patsubst %,-L%,$(DLIBDIR) $(ULIBDIR))
 
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
CPFLAGS = $(MCFLAGS) $(OPT) -std=c99 -gdwarf-2 -mthumb -fno-strict-aliasing -fomit-frame-pointer -Wall -Wstrict-prototypes -fverbose-asm -Wa,-ahlms=$(OBJDIR)/$(<:.c=.lst) $(DEFS)
LDFLAGS = $(MCFLAGS) -mthumb -nostartfiles -T$(LDSCRIPT) -Wl,-Map=$(FULL_PRJ).map,--cref,--no-warn-mismatch $(LIBDIR)
 
# Generate dependency information
CPFLAGS += -MD -MP -MF .dep/$(@F).d
 
#
# makefile rules
#
bootloadable: normal_build
	@echo *********************************************************************************
	@echo *                                 WARNING!
	@echo *********************************************************************************
	@echo *
	@echo * If you switched from building for flash programming to building for bootloader 
	@echo * programming without doing a 'clean', then the built image will not run.
	@echo * 
	@echo * Start with a 'make clean', then do a 'make bootloadable'
	@echo *
	@echo *********************************************************************************
	
all: normal_build
	@echo *********************************************************************************
	@echo *                                 WARNING!
	@echo *********************************************************************************
	@echo *
	@echo * If you switched from building for bootloader programming to building for flash 
	@echo * programming without doing a 'clean', then the built image will not run.
	@echo * 
	@echo * Start with a 'make clean', then do a 'make all'
	@echo *
	@echo *********************************************************************************


normal_build: dirs $(OBJS) elfhexbin
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

elfhexbin: $(OBJS)
	$(CC) $(OBJS) $(LDFLAGS) $(LIBS) -o $(FULL_PRJ).elf
	$(HEX) $(FULL_PRJ).elf $(FULL_PRJ).hex
	$(BIN) $(FULL_PRJ).elf $(FULL_PRJ).bin

unixclean:
	# CAUTION: NOT UPDATED!
	# If building on a Linux box, or using different version of make, you may need
	# to make this match the "clean" target.
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
	-rmdir $(OBJDIR) /S /Q   
	-del $(FULL_PRJ).elf
	-del $(FULL_PRJ).map
	-del $(FULL_PRJ).hex
	-del $(FULL_PRJ).bin
	-del $(PROJECT_BL)_rom.elf
	-del $(PROJECT_BL)_rom.map
	-del $(PROJECT_BL)_rom.hex
	-del $(PROJECT_BL)_rom.bin
	-del $(SRC:.c=.c.bak)
	-del $(ASRC:.s=.s.bak)
	-rmdir .dep /S /Q


# Used for debugging the makefile variables.
dump:
	@echo Dumping out some build variables:
	@echo -------------------------
	@echo SRC:  $(SRC)
	@echo -------------------------
	@echo ASRC: $(ASRC)
	@echo -------------------------
	@echo OBJS: $(OBJS)
	@echo -------------------------
	@echo DIRS: $(DIRS)
	@echo UINCDIR: $(UINCDIR)	
	@echo -------------------------
	@echo Your GCC version:
	$(CC) --version
	@echo Your make version:
	make --version

# *** EOF ***
