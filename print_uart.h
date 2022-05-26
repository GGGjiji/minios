#ifndef PRINTK_UART_H
#define PRINTK_UART_H 1
int print_uart(const char *fmt,...);
void cgashow(char s);
void uart_early_init(void);
#endif
