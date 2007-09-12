/* Set the entry point, warning if we mess up. */
ENTRY(__vec_reset);

/* Setup the memory layout. */
MEMORY
{
	flash	:	ORIGIN = 0x100000,	LENGTH = 256K
	ram		:	ORIGIN = 0x200000,	LENGTH = 64K
}

/* Provide some information about where the RAM starts and ends. */
__ram_start	= 0x200000;
__ram_end	= 0x20fffc;

/* The sections of the object file. */
SECTIONS
{
	/* Start at flash. */
	. = 0;

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

	/* Provide and _end variable, usefull for malloc & co. */
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
	} > flash
}
