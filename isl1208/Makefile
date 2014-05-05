LINUXROOT := /home/Hi3520D_SDK_V1.0.2.0/osdrv/kernel/linux-3.0.y
PWD := $(shell pwd)
obj-m := gpio.o
gpio-y += hi_gpio.o

default:
	@make -C $(LINUXROOT) M=$(PWD) modules 
clean: 
	@rm *.o *.ko -rf
	@make -C $(LINUXROOT) M=$(PWD) clean 
