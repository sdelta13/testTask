obj-m += dmp.o
all:install
compile:
	make -C /lib/modules/$(shell uname -r)/build M=$(PWD) modules

clean:
	make -C /lib/modules/$(shell uname -r)/build M=$(PWD) clean
install: compile
	sudo rmmod dmp.ko -f; sudo insmod dmp.ko && echo installed
status:
	dmesg
testInit:
	size=100
	dmsetup create zero1 --table "0 $size zero"
