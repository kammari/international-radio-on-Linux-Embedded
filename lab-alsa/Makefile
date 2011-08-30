
LIBS=/home/lukuen/arm-linux/lib
INCLUDES=/home/lukuen/arm-linux/include

pc : main.c
	gcc main.c -lasound -o a.out 

arm : main.c
	arm-none-linux-gnueabi-gcc main.c -o alsatest -I$(INCLUDES) -L$(LIBS) -lasound

clean : 
	rm -rf a.out alsatest
