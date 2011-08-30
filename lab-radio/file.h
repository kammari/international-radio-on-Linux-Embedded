#ifndef FILE_H
#define FILE_H

#include <pthread.h>

typedef struct file_struct {
	FILE *					fp;
	int 					eof;
	long 					size;
	pthread_mutex_t			lock;
} file_strt,*file_t ;


file_t file_open(const char * file_name, const char * mode);
int file_close(file_t file);

size_t file_read(void *user_buffer, long offset, size_t count, file_t file);
size_t file_write(void *user_buffer, long offset, size_t count, file_t file);

long get_file_size(file_t file);

int is_endof_file(file_t file);
int file_flush(file_t file);

#endif
