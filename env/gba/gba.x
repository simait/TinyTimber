/* Set the entry point, warning if we mess up. */
ENTRY(__vec_reset);

/* Setup the memory layout. */
MEMORY
{
	sflash	:	ORIGIN = 0x00000000,	LENGTH = 16K
	eram	:	ORIGIN = 0x02000000,	LENGTH = 256K
	iram	:	ORIGIN = 0x03000000,	LENGTH = 32K

	/*
	 * There are actually 3 different types of roms located at 3 distinct
	 * addresses and the only difference is the wait state or access time
	 * for the rom. In this example we'll just use the first/fastest address
	 * range for our simulated rom.
	 */
	flash	:	ORIGIN = 0x08000000,	LENGTH = 32M
}

/* Provide some information about where some areas start and end. */
__sflash_start = 0x00000000;
__sflash_end   = 0x00004000;
__eram_start   = 0x02000000;
__eram_end     = 0x02040000;
__iram_start   = 0x03000000;
__iram_end     = 0x03008000;
__flash_start  = 0x08000000;
__flash_end    = 0x0a000000;

/* Provide the start and end of the ram, could be anywhere really. */
__ram_start    = __iram_start;
__ram_end      = __iram_end;

/* The sections of the object file. */
SECTIONS
{
	.bios :
	{
		. = __sflash_start;
		*(.bios);
		. = __sflash_end;
	} > sflash =0

	/* Data section, copied by our crt from flash to ram.
     *
	 * NOTE:
	 *	The .data section must come BEFORE the .text section. This is just
	 *	because we wish for the .vectors to be located at offset 0 in the flash
	 *	where the initializer data is stored.
	 */

	. = __flash_start;
	
	/* Text section. */
	.text :
	{
		*(.vectors);
		*(.text);
		*(.rodata);
		*(.rodata*);
		*(.got*);
		*(.data.rel*);
		*(.glue_7);
		*(.glue_7t);
		. = ALIGN(4);
	} >  flash =0

	.data :
	{
		/* This is where the data starts. */
		*(.data);
	} > iram AT > flash =0

	/* We need this to copy the data. */
	__data_start = LOADADDR(.data);
	__data_end   = LOADADDR(.data) + SIZEOF(.data);

	/* 
	 * BSS section, intialized to zero. This doesn't strictly have to be
	 * before the .text section but let's keep the RAM stuff together.
	 */
	.bss :
	{
		__bss_start = .;
		*(.bss);
		. = ALIGN(4);
		*(COMMON);
		. = ALIGN(4);
		__bss_end = .;
	} > iram

	/* Provide _end variable, usefull for malloc & co. */
	_end = ADDR(.bss) + SIZEOF(.bss);
}
