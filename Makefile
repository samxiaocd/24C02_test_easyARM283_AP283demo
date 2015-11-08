
CFLAGS += 
obj := i2c_24c02_test
src := main.c
CC  := /home/samxiao/gcc-4.4.4-glibc-2.11.1-multilib-1.0/arm-fsl-linux-gnueabi/bin/arm-fsl-linux-gnueabi-gcc

$(obj): $(src)
	$(CC) $(CFLAGS)  -o $@  $^

.PHONY: clean
clean:
	-rm i2c_24c02_test
