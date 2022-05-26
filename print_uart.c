#include "types.h"
#include "x86.h"
#include "memlayout.h"
#define COM1	0x3f8

void uart_putc(int c);

void uart_early_init(void)
{
  // Turn off the FIFO
  outb(COM1+2, 0);

  // 9600 baud, 8 data bits, 1 stop bit, parity off.
  outb(COM1+3, 0x80);    // Unlock divisor
  outb(COM1+0, 115200/9600);
  outb(COM1+1, 0);
  outb(COM1+3, 0x03);    // Lock divisor, 8 data bits.
  outb(COM1+4, 0);
  outb(COM1+1, 0x01);    // Enable receive interrupts.

  // If status is 0xFF, no serial port.
  if(inb(COM1+5) == 0xFF)
    return;
}

void uart_putc(int c)
{
	outb(COM1+0, c);
}

void uart_putint(int xx, int base, int sgn)
{
  static char digits[] = "0123456789ABCDEF";
  char buf[16];
  int i, neg;
  uint x;

  neg = 0;
  if(sgn && xx < 0){
    neg = 1;
    x = -xx;
  } else {
    x = xx;
  }

  i = 0;
  do{
    buf[i++] = digits[x % base];
  }while((x /= base) != 0);
  if(neg)
    buf[i++] = '-';
  while(--i >= 0){
    uart_putc(buf[i]);
  }
}

int print_uart(const char *fmt, ...)
{
    uart_early_init();
	int state,i,c;
	state = 0;
	uint *ap;
	ap = (uint*)(void*)&fmt + 1;
	for(i = 0; fmt[i];i ++)
	{
		c = fmt[i] & 0xff;
		if(state == 0){
			if(c == '%'){
				state = '%';		
			}else{
				uart_putc(c);
			}
		} else if(state == '%'){
			if(c == 'd'){
				uart_putint(*ap,10,1);
				ap++;
			}
			else if(c == 'x'){
				uart_putint(*ap,16,1);
				ap++;
			}
			else if(c == 's'){
				char *s = (char*)*ap;
				ap++;
				if(s == 0)
				  s = "(null)";
				while(*s != 0){
				  uart_putc(*s);
				  s++;
				}
			}
			else {
				uart_putc('%');
				uart_putc(c);
			}
			state = 0;
		}
	}
    return 0;
}
/*
void panic(char *s)
{
	print_uart(s);
}
*/
