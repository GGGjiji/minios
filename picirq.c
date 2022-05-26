#include "types.h"
#include "x86.h"
#include "traps.h"
#include "print_uart.h"

// I/O Addresses of the two programmable interrupt controllers
#define PIC_M_CTRL    0x20
#define PIC_M_DATA    0x21
#define PIC_S_CTRL    0xa0
#define PIC_S_DATA    0xa1

// Don't use the 8259A interrupt controllers.  Xv6 assumes SMP hardware.
void
picinit(void)
{
    //  init pic_m
    outb(PIC_M_CTRL,0x11);
    outb(PIC_M_DATA,0x20);
    outb(PIC_M_DATA,0x04);
    outb(PIC_M_DATA,0x01);

    //  init pic_s
    outb(PIC_S_CTRL,0x11);
    outb(PIC_S_DATA,0x28);
    outb(PIC_S_DATA,0x02);
    outb(PIC_S_DATA,0x01);

    //  init
    outb(PIC_M_DATA,0xfc);
    outb(PIC_S_DATA,0xff);
}

//PAGEBREAK!
// Blank page.
