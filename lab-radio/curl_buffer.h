#ifndef CURL_BUFFER_H
#define CURL_BUFFER_H


#include <stdio.h>

#include "file.h"

typedef struct curl_buffer {
    file_t      file;
    long        offset;
} curl_buffer_strt, *curl_buffer_t;

curl_buffer_t new_curl_buffer(file_t file);
void destroy_curl_buffer(curl_buffer_t cbuf);

#endif
