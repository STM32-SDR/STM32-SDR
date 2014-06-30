/*

	STM32F4 Discovery Board linker script
	
	Used for setting up the image for bootloader support.

*/

MEMORY
{
  /* This one for "bootloader" programming*/
  FLASH		(rx)	: ORIGIN = 0x08008000, LENGTH = 992K
  RAMCCM	(xrw)	: ORIGIN = 0x10000000, LENGTH = 64K
  RAM 		(xrw)	: ORIGIN = 0x20000000, LENGTH = 128K
}

ENTRY(Reset_Vector)

top_of_stack	=	ORIGIN(RAM) + LENGTH(RAM);

/*
Heap_Size	=	0;

Heap to 16K, set for the GUI framework which allocates on the heap.
*/
Heap_Size	=	0x3E80;

Stack_Size	=	0x400;

SECTIONS
{
  .Vector_Table :
  {
    . = ALIGN(4);
    KEEP(*(.Vector_Table))
    . = ALIGN(4);
  } >FLASH

  .text :
  {
    . = ALIGN(4);
    *(.text)
    *(.text*)
    *(.rodata)
    *(.rodata*)
    *(.glue_7)
    *(.glue_7t)
	*(.eh_frame)

    KEEP (*(.init))
    KEEP (*(.fini))

    . = ALIGN(4);
    _end_of_code = .;
  } >FLASH


   .ARM.extab   : { *(.ARM.extab* .gnu.linkonce.armextab.*) } >FLASH
 
   .ARM : {
    __exidx_start = .;
      *(.ARM.exidx*)
      __exidx_end = .;
    } >FLASH

  .preinit_array     :
  {
    PROVIDE_HIDDEN (__preinit_array_start = .);
    KEEP (*(.preinit_array*))
    PROVIDE_HIDDEN (__preinit_array_end = .);
  } >FLASH
  .init_array :
  {
    PROVIDE_HIDDEN (__init_array_start = .);
    KEEP (*(SORT(.init_array.*)))
    KEEP (*(.init_array*))
    PROVIDE_HIDDEN (__init_array_end = .);
  } >FLASH
  .fini_array :
  {
    PROVIDE_HIDDEN (__fini_array_start = .);
    KEEP (*(.fini_array*))
    KEEP (*(SORT(.fini_array.*)))
    PROVIDE_HIDDEN (__fini_array_end = .);
  } >FLASH

  /* used by the startup to initialize data */
  _sidata = .;

  /* Initialized data sections goes into RAM, load LMA copy after code */
  .data : AT ( _sidata )
  {
    . = ALIGN(4);
    _sdata = .;        /* create a global symbol at data start */
    *(.data)           /* .data sections */
    *(.data*)          /* .data* sections */

    . = ALIGN(4);
    _edata = .;        /* define a global symbol at data end */
  } >RAM

  /* Uninitialized data section */
  . = ALIGN(4);
  .bss :
  {
    /* This is used by the startup in order to initialize the .bss secion */
    _sbss = .;         /* define a global symbol at bss start */
    __bss_start__ = _sbss;
    *(.bss)
    *(.bss*)
    *(COMMON)

    . = ALIGN(4);
    _ebss = .;         /* define a global symbol at bss end */
    __bss_end__ = _ebss;
  } >RAM

  /* User_heap_stack section, used to check that there is enough RAM left */
  ._user_heap_stack :
  {
    . = ALIGN(4);
    PROVIDE ( end = . );
    PROVIDE ( _end = . );
    . = . + Heap_Size;
    . = . + Stack_Size;
    . = ALIGN(4);
  } >RAM
  
  PROVIDE(__HEAP_START = _end );

  /* CORE COUPLED MEMORY section, code must be located here explicitly            */
  /* Example: extern int foo(void) __attribute__ ((section (".ccmtext"))); */
  .ramccm :
  {
    *(.ccmtext)        /* .ccmtext sections (code) */
    *(.ccmtext*)       /* .ccmtext* sections (code)  */
    *(.ccmrodata)      /* ccm read-only data (constants) */
    *(.ccmrodata*)
  } >RAMCCM

  /* Remove information from the standard libraries */
  /DISCARD/ :
  {
    libc.a ( * )
    libm.a ( * )
    libgcc.a ( * )
  }

  .ARM.attributes 0 : { *(.ARM.attributes) }
}
