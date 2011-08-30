#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>

#include "file.h"


file_t file_open(const char * file_name, const char *mode)
{
	FILE *filep = fopen(file_name, mode);
	if (filep == NULL) {
		fprintf(stderr, "file_open() : open file error.\n");
		return NULL;
	}

	file_t file = (file_t)malloc(sizeof(file_strt));
	file->fp = filep; 
	file->eof = 0;
	file->size = 0;
	pthread_mutex_init(&file->lock, NULL);

	return file;
}

int file_close(file_t file)
{
	int ret;
	ret = fclose(file->fp);	
	if (ret != 0) {
		fprintf(stderr, "close_file() : close file error.\n");
	}
	pthread_mutex_destroy(&file->lock);	
	free(file);

	return ret;
}

size_t file_read(void *user_buffer, long offset, size_t count, file_t file)
{
	size_t read_size, remain, exact;
	int ret;

	pthread_mutex_lock(&file->lock);

	if (file == NULL || file->fp == NULL) {
		fprintf(stderr, "file_read() : file not init...\n");
		goto err;
	}

	remain = file->size - offset;
	if (remain <= 0) {
//		fprintf(stderr, "file size is not huge enough.\n");
		goto err;
	}

	if (remain < count) {
		read_size = remain;
	} else {
		read_size = count;
	}

	ret = fseek(file->fp, offset, SEEK_SET);	
	if (ret) {
		printf("ERROR : file_read() fseek error.\n");
		goto err;
	}
printf("offset = %ld,read_size = %d\n", offset, read_size);
	exact = fread(user_buffer, 1, read_size, file->fp);

err:
	pthread_mutex_unlock(&file->lock);
	if (ret) {
		return 0;
	}
	
	return exact;
}

size_t file_write(void *user_buffer, long offset, size_t count, file_t file)
{
	size_t write_size;
	int ret;
	if (file == NULL) {
		fprintf(stderr, "ERROR : file_write() file handle in null.\n");
		return -1;
	}

	pthread_mutex_lock(&file->lock);
	if (file->fp == NULL) {
		fprintf(stderr, "ERROR : file_write() file handle in null.\n");
		write_size = 0;
		goto err;
	}

    ret = fseek(file->fp, offset, SEEK_SET);
    if (ret) {
        printf("ERROR : file_read() fseek error.\n");
        goto err;
    }

	write_size = fwrite(user_buffer, 1, count, file->fp);
	file->size += write_size;
	
	printf("file->size = %ld, write_size = %d, offset = %ld.\n", file->size, write_size, offset);

err:
	pthread_mutex_unlock(&file->lock);
	
	return write_size;
}

long get_file_size(file_t file)
{
	if (file != NULL) {
		return file->size;
	} 
	return -1;
}

int is_endof_file(file_t file)
{
	if (file != NULL) {
		return file->eof;
	}
	return -1;
}

int file_flush(file_t file)
{
	fflush(file->fp);
	return 0;
}

