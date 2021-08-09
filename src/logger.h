#include <stdio.h>
#include <string.h>
#include <time.h>
#include <stdlib.h>
#include <pthread.h>

void log_this(char *message, char *file_path, pthread_mutex_t *mx);
char *get_date_string();