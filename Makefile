# Default make target
.PHONY: all
all: miniOS.img	

QEMU=qemu-system-i386
CC=gcc
LD=ld
OBJCOPY=objcopy
OBJDUMP=objdump
CFLAGS = -fno-pic -static -fno-builtin -fno-strict-aliasing -O2 -Wall -MD -ggdb -m32 -Werror -fno-omit-frame-pointer
CFLAGS += $(shell $(CC) -fno-stack-protector -E -x c /dev/null >/dev/null 2>&1 && echo -fno-stack-protector)
ASFLAGS = -m32 -gdwarf-2 -Wa,-divide
# FreeBSD ld wants ``elf_i386_fbsd''
LDFLAGS += -m $(shell $(LD) -V | grep elf_i386 2>/dev/null | head -n 1)

ifneq ($(shell $(CC) -dumpspecs 2>/dev/null | grep -e '[^f]no-pie'),)
CFLAGS += -fno-pie -no-pie
endif
ifneq ($(shell $(CC) -dumpspecs 2>/dev/null | grep -e '[^f]nopie'),)
CFLAGS += -fno-pie -nopie
endif

OBJS=	\
	main.o		\
	kalloc.o	\
	string.o	\
	vm.o		\
	picirq.o	\
	console.o	\
	file.o		\
	print_uart.o	\
	trapasm.o	\
	trap.o		\
	vectors.o	\
	kbd.o		\
	proc.o		\
	swtch.o		\


miniOS.img: bootblock kernel
	dd if=/dev/zero of=miniOS.img count=10000
	dd if=bootblock of=miniOS.img conv=notrunc
	dd if=kernel of=miniOS.img seek=1 conv=notrunc

bootblock: bootasm.S bootmain.c
	echo $(CFLAGS)
	$(CC) $(CFLAGS) -fno-pic -O -nostdinc -I. -c bootmain.c
	$(CC) $(CFLAGS) -fno-pic -nostdinc -I. -c bootasm.S
	$(LD) $(LDFLAGS) -N -e start -Ttext 0x7C00 -o bootblock.o bootasm.o bootmain.o
	$(OBJDUMP) -S bootblock.o > bootblock.asm
	$(OBJCOPY) -S -O binary -j .text bootblock.o bootblock
	./sign.pl bootblock

initcode: initcode.S
	$(CC) $(CFLAGS) -nostdinc -I. -c initcode.S
	$(LD) $(LDFLAGS) -N -e start -Ttext 0 -o initcode.out initcode.o
	$(OBJCOPY) -S -O binary initcode.out initcode
	$(OBJDUMP) -S initcode.o > initcode.asm

kernel: $(OBJS) entry.o kernel.ld initcode
	$(LD) $(LDFLAGS) -T kernel.ld -o kernel entry.o $(OBJS) -b binary initcode
	$(OBJDUMP) -S kernel > kernel.asm
	$(OBJDUMP) -t kernel | sed '1,/SYMBOL TABLE/d; s/ .* / /; /^$$/d' > kernel.sym
	
vectors.S: vectors.pl
	./vectors.pl > vectors.S

clean: 
	rm -f *.tex *.dvi *.idx *.aux *.log *.ind *.ilg \
	*.o *.d *.asm *.sym vectors.S bootblock entryother \
	initcode initcode.out kernel xv6.img 

GDBPORT = $(shell expr `id -u` % 5000 + 25000)
QEMUGDB = $(shell if $(QEMU) -help | grep -q '^-gdb'; \
	then echo "-gdb tcp::$(GDBPORT)"; \
	else echo "-s -p $(GDBPORT)"; fi)
	

QEMUOPTS = -drive file=miniOS.img,index=0,media=disk,format=raw -smp 1 -m 512 $(QEMUEXTRA)

qemu: miniOS.img
	$(QEMU) -serial mon:stdio $(QEMUOPTS)

qemugdb : miniOS.img
	$(QEMU) -serial mon:stdio $(QEMUOPTS) -S $(QEMUGDB)
