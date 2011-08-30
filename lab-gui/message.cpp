#include <stdio.h>
#include <pthread.h>
#include "message.h"

char input_char;
pthread_t thr_stdin;
pthread_t thr_message;

pthread_mutex_t inputlock;
pthread_mutex_t inputsem;

void* stdin_func(void *data);

char igetchar() {
    char c;
    pthread_mutex_lock(&inputsem);
    pthread_mutex_lock(&inputlock);
    c = input_char;
    pthread_mutex_unlock(&inputlock);
    return c;
}

int iputchar(char c) {
    pthread_mutex_lock(&inputlock);
    input_char = c;
    pthread_mutex_unlock(&inputsem);
    pthread_mutex_unlock(&inputlock);
    return 0;
}

int imessage_init() {
	pthread_create(&thr_stdin, NULL, stdin_func, NULL);	
	pthread_mutex_init(&inputlock, NULL);
	pthread_mutex_init(&inputsem, NULL);
	pthread_mutex_lock(&inputsem);
	return 0;
}

void* stdin_func(void *data) {
    while (1) {
        char c = getc(stdin);
        if ( c == '\n') {
            continue;
        }
        pthread_mutex_lock(&inputlock);
        input_char = c;
        pthread_mutex_unlock(&inputsem);    
        pthread_mutex_unlock(&inputlock);
    }
    return (void*)0;
}

