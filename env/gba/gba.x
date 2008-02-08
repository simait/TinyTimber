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
__sflash_end   = 0x00003fff;
__eram_start   = 0x02000000;
__eram_end     = 0x0203ffff;
__iram_start   = 0x03000000;
__iram_end     = 0x03007fff;
__flash_start  = 0x08000000;
__flash_end    = 0x09ffffff;

/* Provide the start and end of the ram, could be anywhere really. */
__ram_start    = __iram_start;
__ram_end      = __iram_end;

/* The sections of the object file. */
SECTIONS
{
	/* Start at flash. */
	. = 0;

	.bios :
	{
		*(.bios);
	} > sflash

	/* Data section, copied by our crt from flash to ram.
     *
	 * NOTE:
	 *	The .data section must come BEFORE the .text section. This is just
	 *	because we wish for the .vectors to be located at offset 0 in the flash
	 *	where the initializer data is stored.
	 */
	.data :
	{
		/* This is where the data starts. */
		*(.vectors);
		*(.data);
		*(.forced_ram); /* This is the forced ram section. */
		. = ALIGN(4);
	} > ram AT > flash

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
	} > ram

	/* Provide _end variable, usefull for malloc & co. */
	_end = ADDR(.bss) + SIZEOF(.bss);

	/* Text section. */
	.text :
	{
		*(.text);
		*(.rodata);
		*(.rodata*);
		*(.glue_7);
		*(.glue_7t);
		. = ALIGN(4);
	} >  flash
}
