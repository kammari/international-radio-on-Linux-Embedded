#include <unistd.h>
#include <pthread.h>

#include "decoder.h"
#include "curl.h"
#include "message.h"

pthread_t thr_iplayer;

void* play_func(void *data) {

	char c;
	file_t fp = file_open("77.mp3", "w+");
	curl_init(fp);	
	mad_init(fp);
	imessage_init();
printf("mad curl init ok\n");

	while ((c = igetchar()) != EOF) {
		printf("s:start, c:stop, g:continue, n:next.\n");
		switch ( c ) {
			case 's':
				start_curl();
				start_mad();
				break;
			case 'c':
				stop_mad();
				break;
			case 'g':
				continue_mad();
				break;
			case 'n':
				next_curl();
				start_curl();
				next_mad();	
				start_mad();
				break;
			case 'q':
				goto exit;			
			case '\n':
				break;		
		}
	}

exit:
    return (void*)0;
}

