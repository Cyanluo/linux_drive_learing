

.text 
.global _start

_start:

	//关闭看门狗
	ldr r0, =0x53000000
	mov r1, #0
	str r1, [r0]

	//时钟配置FCLK:400M HCLK:100M PCLK:50M
	//配置HDIV、PDIV
	ldr r0, =0x4C000014
	mov r1, #0x5
	str r1, [r0]
	
	//配置为异步模式
	mrc p15,0,r0,c1,c0,0 
 	orr r0,r0,#0xc0000000   //R1_nF:OR:R1_iA
 	mcr p15,0,r0,c1,c0,0

	//配置MPLL,使FCLK为400M
	ldr r0, =0x4C000004
	ldr r1, =((92<<12) | (1<<4) | (1<<0))
	str r1, [r0]

	//设置栈 sp 自动识别nor|nand启动
	//nor flash 的读写需要一定格式，不能直接读写，由此判断启动模式
	mov r1, #0 //r1=0
	ldr r0, [r1]//[0]->r0
	str r1, [r1]//r1->[0]
	ldr r2, [r1]//[0]->r2
	cmp r1, r2//比较r1,r2
	ldr sp, =0x40000000+4096//nor启动
	moveq sp, #4096//如果r1=r2，nand启动
	streq r0, [r1]//还原0地址数据

	//初始化SDRAM
	bl sdram_init

	//代码重定位
	mov r0, #0
	ldr r1, =_start
	ldr r2, =bass_start

copy:
	ldr  r3, [r0]
	str  r3, [r1]
	add  r0, r0, #4
	add  r1, r1, #4
	cmp	 r1, r2
	ble  copy

	ldr r1, =bass_end
	mov r0, #0

	//清除bass段数据
clean:
	str r0, [r2]
	add r2, r2, #4
	cmp r2, r1
	ble clean	

	//用绝对地址跳转指令，跳到SDRAM中去执行
	ldr  pc, =main

halt:
	b halt

	
	