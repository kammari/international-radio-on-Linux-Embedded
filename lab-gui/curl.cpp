#include <stdio.h>
#include <unistd.h>
#include <curl/curl.h>
#include <pthread.h>
#include <string.h>

#include "curl.h"
#include "flag.h"
#include "file.h"
#include "curl_buffer.h"

static flag_t *curl_flag;
static pthread_t curl_thread;
static pthread_mutex_t	curl_start_lock;

static size_t write_data(void *ptr, size_t size, size_t nmemb, void *stream);

const char * get_music_url()
{
	static int i = 0;
	i++;
	if (i % 2 == 1) {
		return "http://127.0.0.1:8080/examples/77.mp3";
	}
	else
		return "http://127.0.0.1:8080/examples/blg.mp3";
}

static void *curl_func(void * share_file) 
{
	//read music from music box

	CURL *curl;
	CURLcode res;
	
	const char * file_name;

	if (share_file == NULL) {
		fprintf(stderr, "Fatal Error: share file not initialized in curl_func().\n");
	}

	file_t curl_fd = (file_t)share_file;

	curl_buffer_t cbuf = new_curl_buffer(curl_fd);

	curl_flag = new_flag();
	curl_global_init(CURL_GLOBAL_ALL);

	while (1) {
		// get the music file name and create it if it haven't already exist.
		pthread_mutex_lock(&curl_start_lock);
		
		cbuf->offset = 0;
		cbuf->file->eof = 0;
		cbuf->file->size = 0;
		set_flag(curl_flag, '\0');

		file_name = get_music_url();
		printf("file name = %s\n", file_name);		

		curl = curl_easy_init();
		curl_easy_setopt (curl, CURLOPT_URL, file_name);
		curl_easy_setopt (curl, CURLOPT_WRITEFUNCTION, write_data);
		curl_easy_setopt (curl, CURLOPT_WRITEDATA, cbuf);
		//  curl_easy_setopt(curl, CURLOPT_TIMEOUT, 9);            
		res = curl_easy_perform(curl);
		if(CURLE_OK != res){
			printf("curl error...\n");
		}

		cbuf->file->eof = 1;

		curl_easy_cleanup(curl);

		if (is_going_to_close(curl_flag)) {
			goto quit;
		}
	}

quit:
	destroy_curl_buffer(cbuf);	
	destroy_flag(curl_flag);
	return (void*)0;
}

int curl_init(file_t share_file)
{
	int ret;
	ret = pthread_mutex_init(&curl_start_lock, NULL);
	if (ret != 0) {
		fprintf(stderr, "start_curl() : pthread_mutex_init start lock error.\n");
	}
	ret = pthread_mutex_lock(&curl_start_lock);

	ret = pthread_create(&curl_thread, NULL, curl_func, share_file);
	if (ret != 0) {
		fprintf(stderr, "start_url() : pthread create error.\n");
		return -1;
	}

	return 0;
}

int start_curl()
{
	pthread_mutex_unlock(&curl_start_lock);
	return 0;
}

int next_curl()
{
	set_flag(curl_flag, 'n');
	return 0;
} 

int close_curl()
{
	set_flag(curl_flag, 'q');
	return 0;
}

int wait_curl()
{
	pthread_join(curl_thread, NULL);
	return 0;
}

static size_t write_data(void *ptr, size_t size, size_t nmemb, void *stream)
{
	size_t write_size;
//        printf("write_data %d\n",i);
/*
    if (i == 5) {
		sleep(5);
    }
    i++;
*/
    curl_buffer_t cbuf = (curl_buffer_t)stream;
    write_size = file_write(ptr, cbuf->offset, size*nmemb, cbuf->file);
	if (write_size != size*nmemb) {
		printf("write _ data error......\n");
	}
	cbuf->offset += write_size;
    file_flush(cbuf->file);

	if (is_going_to_next(curl_flag) || is_going_to_close(curl_flag)) {
		return -1;
	}

    return write_size;
}

