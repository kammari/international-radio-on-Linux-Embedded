
CC=arm-none-linux-gnueabi-gcc
LIBS=/home/lukuen/arm-linux/lib
INCLUDES=/home/lukuen/arm-linux/include

main : main.c
	gcc -o curl_test main.c -lcurl

arm : main.c
	$(CC) main.c -o curl-arm -I$(INCLUDES) -L$(LIBS) -lcurl

clean : 
	rm -rf curl_test curl-arm
