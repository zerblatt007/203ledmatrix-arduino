# Introduction #

> - What you need to do to upload with Atmel stk500 ISP programmer
> - What need to be changed so Arduino IDE accepts and compiles for ATmega88


# STK500 #
Add to java/hardware/arduino/programmers.txt ```

stk500.name=stk500
stk500.communication=serial
stk500.protocol=stk500v2
stk500.speed=115200
```

# Boards #
Add to java/hardware/arduino/boards.txt ```

##############################################################
matrix88.name=LED Matrix 88

matrix88.upload.using=stk500
matrix88.upload.protocol=stk500v2
matrix88.upload.maximum_size=7168
matrix88.upload.speed=115200
matrix88.bootloader.path=atmega8
matrix88.bootloader.file=ATmegaBOOT.hex
matrix88.build.mcu=atmega88p
matrix88.build.f_cpu=8000000L
matrix88.build.core=arduino
##############################################################
```