

.text 
.global _start

_start:

	//�رտ��Ź�
	ldr r0, =0x53000000
	mov r1, #0
	str r1, [r0]

	//ʱ������FCLK:400M HCLK:100M PCLK:50M
	//����HDIV��PDIV
	ldr r0, =0x4C000014
	mov r1, #0x5
	str r1, [r0]
	
	//����Ϊ�첽ģʽ
	mrc p15,0,r0,c1,c0,0 
 	orr r0,r0,#0xc0000000   //R1_nF:OR:R1_iA
 	mcr p15,0,r0,c1,c0,0

	//����MPLL,ʹFCLKΪ400M
	ldr r0, =0x4C000004
	ldr r1, =((92<<12) | (1<<4) | (1<<0))
	str r1, [r0]

	//����ջ sp �Զ�ʶ��nor|nand����
	//nor flash �Ķ�д��Ҫһ����ʽ������ֱ�Ӷ�д���ɴ��ж�����ģʽ
	mov r1, #0 //r1=0
	ldr r0, [r1]//[0]->r0
	str r1, [r1]//r1->[0]
	ldr r2, [r1]//[0]->r2
	cmp r1, r2//�Ƚ�r1,r2
	ldr sp, =0x40000000+4096//nor����
	moveq sp, #4096//���r1=r2��nand����
	streq r0, [r1]//��ԭ0��ַ����

	//��ʼ��SDRAM
	bl sdram_init

	//�����ض�λ
	ldr r0, =data_load_addr
	ldr r1, =start_addr
	ldr r2, =end_addr

copy:
	ldrb r3, [r0]
	strb r3, [r1]
	add  r0, r0, #1
	add  r1, r1, #1
	cmp	 r1, r2
	bne  copy
	
	bl main

halt:
	b halt

	
	