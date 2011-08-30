#ifndef CURL_H
#define CURL_H

#include <stdio.h>

#include "file.h"

int curl_init(file_t share_file);

int start_curl();
//int stop_curl();
//int continue_curl();
int next_curl();
int close_curl();
int wait_curl();



#endif
