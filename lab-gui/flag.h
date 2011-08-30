#ifndef FLAG_H
#define FLAG_H

#include <pthread.h>

typedef struct flag_strt {
	 pthread_mutex_t lock;
	 char flag;
}flag_t;


flag_t * new_flag();
int destroy_flag(flag_t *pflag);

void set_flag(flag_t *flag, char c);
char get_flag(flag_t *flag);
int is_going_to_next(flag_t *flag);
int is_going_to_close(flag_t *flag);
int is_going_to_start(flag_t *flag);


#endif
