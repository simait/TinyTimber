ENTRY(_start)

MEMORY {
	ram (rw) : o = 0x0000400, l = 31k
	flash (rx) : o = 0x00C0000, l = 256k
}

/* Provide any vector symbols not defined. */
PROVIDE(_vector_0 = _vector_default);
PROVIDE(_vector_1 = _vector_default);
PROVIDE(_vector_2 = _vector_default);
PROVIDE(_vector_3 = _vector_default);
PROVIDE(_vector_4 = _vector_default);
PROVIDE(_vector_5 = _vector_default);
PROVIDE(_vector_6 = _vector_default);
PROVIDE(_vector_7 = _vector_default);
PROVIDE(_vector_8 = _vector_default);
PROVIDE(_vector_9 = _vector_default);
PROVIDE(_vector_10 = _vector_default);
PROVIDE(_vector_11 = _vector_default);
PROVIDE(_vector_12 = _vector_default);
PROVIDE(_vector_13 = _vector_default);
PROVIDE(_vector_14 = _vector_default);
PROVIDE(_vector_15 = _vector_default);
PROVIDE(_vector_16 = _vector_default);
PROVIDE(_vector_17 = _vector_default);
PROVIDE(_vector_18 = _vector_default);
PROVIDE(_vector_19 = _vector_default);
PROVIDE(_vector_20 = _vector_default);
PROVIDE(_vector_21 = _vector_default);
PROVIDE(_vector_22 = _vector_default);
PROVIDE(_vector_23 = _vector_default);
PROVIDE(_vector_24 = _vector_default);
PROVIDE(_vector_25 = _vector_default);
PROVIDE(_vector_26 = _vector_default);
PROVIDE(_vector_27 = _vector_default);
PROVIDE(_vector_28 = _vector_default);
PROVIDE(_vector_29 = _vector_default);
PROVIDE(_vector_30 = _vector_default);
PROVIDE(_vector_31 = _vector_default);
PROVIDE(_vector_32 = _vector_default);
PROVIDE(_vector_33 = _vector_default);
PROVIDE(_vector_34 = _vector_default);
PROVIDE(_vector_35 = _vector_default);
PROVIDE(_vector_36 = _vector_default);
PROVIDE(_vector_37 = _vector_default);
PROVIDE(_vector_38 = _vector_default);
PROVIDE(_vector_39 = _vector_default);
PROVIDE(_vector_40 = _vector_default);
PROVIDE(_vector_41 = _vector_default);
PROVIDE(_vector_42 = _vector_default);
PROVIDE(_vector_43 = _vector_default);
PROVIDE(_vector_44 = _vector_default);
PROVIDE(_vector_45 = _vector_default);
PROVIDE(_vector_46 = _vector_default);
PROVIDE(_vector_47 = _vector_default);
PROVIDE(_vector_48 = _vector_default);
PROVIDE(_vector_49 = _vector_default);
PROVIDE(_vector_50 = _vector_default);
PROVIDE(_vector_51 = _vector_default);
PROVIDE(_vector_52 = _vector_default);
PROVIDE(_vector_53 = _vector_default);
PROVIDE(_vector_54 = _vector_default);
PROVIDE(_vector_55 = _vector_default);
PROVIDE(_vector_56 = _vector_default);
PROVIDE(_vector_57 = _vector_default);
PROVIDE(_vector_58 = _vector_default);
PROVIDE(_vector_59 = _vector_default);
PROVIDE(_vector_60 = _vector_default);
PROVIDE(_vector_61 = _vector_default);
PROVIDE(_vector_62 = _vector_default);
PROVIDE(_vector_63 = _vector_default);

SECTIONS {
	/*
	 * Ram starts at 0x400 but for some reason it does not allow med to
	 * start placing data at 0x400 since it's not inte the ram... Life
	 * is great.
	 */
	__ram_start = 0x500;
	__ram_end = 0x400 + 31k;

	.data __ram_start :	{
		*(.data);
		*(.rodata);		/* Do NOT place in '.text'. */
		*(.rodata.*);	/* Do NOT place in '.text'. */
		*(.plt);		/* Do NOT place in '.text'. */
	} > ram AT > flash

	__data_start = LOADADDR(.data);
	__data_size = SIZEOF(.data);

	.bss : {
		*(.bss);
		*(COMMON);
	} > ram

	__bss_start = ADDR(.bss);
	__bss_size = SIZEOF(.bss);

	.text : {
		*(.text);
		*(.vectors_variable);
	} > flash

	PROVIDE(_end = __bss_start + __bss_size);

	/* Vector offset is fixed. */
	.vectors 0x000FFFDC : {
		*(.vectors_fixed);
	} > flash
}
