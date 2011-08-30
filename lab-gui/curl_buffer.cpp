#include <stdio.h>
#include <stdlib.h>

#include "curl_buffer.h"


curl_buffer_t new_curl_buffer(file_t f)
{
	if (f == NULL) {
		fprintf(stderr, "Fatal Error : file not init in new_curl_buffer().\n");
		return NULL;
	}

	curl_buffer_t cbuf = (curl_buffer_t)malloc(sizeof(curl_buffer_strt));
	if (cbuf == NULL) {
		fprintf(stderr, "Fatal Error : mem is not enouge in new_curl_buffer().\n");
		return NULL;
	}
	cbuf->file = f;
	cbuf->offset = 0;

	return cbuf;
}


void destroy_curl_buffer(curl_buffer_t cbuf)
{
	if (cbuf == NULL) {
		return ;
	}

	file_close(cbuf->file);	
	free(cbuf);
	return;
}


