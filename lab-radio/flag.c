#include <stdio.h>
#include <stdlib.h>

#include "flag.h"

flag_t * new_flag()
{
	flag_t *pflag = (flag_t *)malloc(sizeof(flag_t));
	pflag->flag = '\0';
	pthread_mutex_init(&pflag->lock, NULL);

	return pflag;
}

int destroy_flag(flag_t *pflag)
{
	pthread_mutex_destroy(&pflag->lock);	
	free(pflag);
	return 0;
}

void set_flag(flag_t *pflag, char c)
{
    pthread_mutex_lock(&pflag->lock);
    pflag->flag = c;
    pthread_mutex_unlock(&pflag->lock);
}

char get_flag(flag_t *pflag) {
    char c;
    pthread_mutex_lock(&pflag->lock);
    c = pflag->flag;
    pthread_mutex_unlock(&pflag->lock);
    return c;
}

int is_going_to_next(flag_t *pflag)
{
    char c = get_flag(pflag);
    if (c == 'n')
        return 1;
    else
        return 0;
}

int is_going_to_close(flag_t *pflag)
{
    char c = get_flag(pflag);
    if (c == 'q')
        return 1;
    else
        return 0;
}

int is_going_to_start(flag_t *pflag)
{
    char c = get_flag(pflag);
    if (c == 's')
        return 1;
    else
        return 0;
}

