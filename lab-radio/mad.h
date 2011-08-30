#ifndef MAD_H
#define MAD_H

#include <stdio.h>

#include "file.h"

#define BUFFERSIZE  (1024*512)

typedef struct mad_struct {
	
	file_t 				file;
	unsigned char 		mdata[BUFFERSIZE];
    size_t              live_size;
    struct mad_stream   *stream;
	long 				offset;
		
}mad_strt, *mad_t;

//mad_t * new_mad(const char *file_name);
//int destroy_mad(mad_t* mad);

int mad_init(file_t file);

int start_mad();
int stop_mad();
int continue_mad();
int next_mad();
int close_mad();
int wait_mad();

#endif
