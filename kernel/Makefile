include ../make.conf

ARCHDIR=arch/$(ARCH)

CFLAGS:=$(CFLAGS) -ffreestanding -Wall -Wextra -fbuiltin -fno-omit-frame-pointer -DNDEBUG
CPPFLAGS:=$(CPPFLAGS) -Iinclude -DARCH=\"$(ARCH)\"
LDFLAGS:=$(LDFLAGS)
LIBS:=$(LIBS) -nostdlib -lgcc
NASMFLAGS:=-felf

include $(ARCHDIR)/make.conf

CFLAGS:=$(CFLAGS) $(KERNEL_ARCH_CFLAGS)
CPPFLAGS:=$(CPPFLAGS) $(KERNEL_ARCH_CPPFLAGS)
LDFLAGS:=$(LDFLAGS) $(KERNEL_ARCH_LDFLAGS) -Xlinker -Map=kernel.map
LIBS:=$(LIBS) $(KERNEL_ARCH_LIBS)


.PHONY: all clean install install-kernel
.SUFFIXES: .o .c

KERNEL_OBJS=\
	$(KERNEL_ARCH_OBJS)\
	page_alloc.o\
	printf.o\
	kmain.o

.c.o:
	$(CC) -MD -c $< -o $@ $(CFLAGS) $(CPPFLAGS)

.s.o:
	$(AS) -c $< -o $@

.S.o:
	nasm $(NASMFLAGS) $< -o $@

all: kernel.bin

kernel.bin: $(KERNEL_OBJS) $(ARCHDIR)/linker.ld
	$(CC) -T $(ARCHDIR)/linker.ld -o $@ $(CFLAGS) $(LDFLAGS) $(KERNEL_OBJS) $(LIBS)


install: install-kernel install-headers

install-kernel: kernel.bin
	mkdir -p  $(SYSROOT)/boot
	cp kernel.bin $(SYSROOT)/boot

install-headers:
	mkdir -p $(SYSROOT)/usr/include
	cp -Rv include/* $(SYSROOT)/usr/include/

clean:
	rm -f kernel.bin
	rm -f $(KERNEL_OBJS)
	rm -f $(KERNEL_OBJS:.o=.d) *.d */*.d */*/*.d
	rm -f kernel.map
