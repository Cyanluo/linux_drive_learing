SECTIONS {
	.text	0 : { *(.text) }
	.rodata	: { *(.rodata) }
	.data	0x30000000 : AT(0x800) 
	{ 
		data_load_addr = LOADADDR(.data);
		start_addr = .;
		*(.data)
		end_addr = .;
	}
	.bass : { *(.bass) *.(COMMON)}
}