
CC=arm-none-linux-gnueabi-gcc
LIBS=/home/lukuen/arm-linux/lib
INCLUDES=/home/lukuen/arm-linux/include


mad : curl_buffer.c curl_buffer.h file.c file.h curl.c curl.h main.c flag.h flag.c mad.h mad.c alsa.c alsa.h 
	g++ main.c curl_buffer.c curl.c mad.c alsa.c flag.c file.c -o mad -Wall -lmad -lasound -lcurl

arm : curl_buffer.c curl_buffer.h file.c file.h curl.c curl.h main.c flag.h flag.c mad.h mad.c alsa.c alsa.h
	$(CC) main.c curl_buffer.c curl.c mad.c alsa.c flag.c file.c -I$(INCLUDES) -L$(LIBS) -o mad-arm -lmad -lasound -lcurl
	
clean :
	rm -rf mad mad-arm *.o *.mp3
