#ifndef __UART_H_
#define __UART_H_

void uart0_init();
int putchar(int c);
int getchar(void);
int puts(const char * s);
void printHex(int val);
void print1(void);
void print2(void);

#endif


