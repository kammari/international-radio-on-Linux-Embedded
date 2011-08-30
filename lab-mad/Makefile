
CC=arm-none-linux-gnueabi-gcc
LIBS=/home/lukuen/arm-linux/lib
INCLUDES=/home/lukuen/arm-linux/include


pc : main.c
	gcc main.c -o minimad -L/usr/local/lib -lmad -lasound

arm : main.c
	$(CC) main.c -o minimad-arm -I$(INCLUDES) -L$(LIBS) -lasound -lmad


clean :
	rm -rf minimad minimad-arm
