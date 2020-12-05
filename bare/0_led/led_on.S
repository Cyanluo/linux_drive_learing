
.text
.global _start

_start:

ldr r1, =0x56000050
mov r0, #0x100
str r0, [r1]

ldr r1, =0x56000054
mov r0, #0
str r0, [r1]

halt:
	b halt
