SECTIONS 
{
	. = 0x30000000;

	. = ALIGN(4);
	.text :
	{ *(.text) }

	. = ALIGN(4);
	.rodata :
	{ *(.rodata) }

	. = ALIGN(4);
	.datr :
	{ *(.data) }

	. = ALIGN(4);
	bass_start = .;
	.bass :
	{ *(.bass) *(.COMMON)}
	bass_end = .;
}