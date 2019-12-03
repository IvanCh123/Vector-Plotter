obj-m+=VFunction_ins.o
VFunction_ins-objs := VFunction.o charProcessor.o floatingPoint.o getAxisValues.o

obj-m+=VFuntionDev.o

EXTRA_CFLAGS += -I${PWD}/include

all:	
	yasm -g dwarf2 -f elf64 charProcessor.asm -l charProcessor.lst
	yasm -g dwarf2 -f elf64 floatingPoint.asm -l floatingPoint.lst
	yasm -g dwarf2 -f elf64 getAxisValues.asm -l getAxisValues.lst
	make -C /lib/modules/$(shell uname -r)/build/ M=$(PWD) modules

install:
	sudo insmod VFunction_ins.ko
	sudo insmod VFuntionDev.ko
	

uninstall:
	sudo rmmod VFuntionDev.ko
	sudo rmmod VFunction_ins.ko
	
clean:
	make -C /lib/modules/$(shell uname -r)/build/ M=$(PWD) clean
