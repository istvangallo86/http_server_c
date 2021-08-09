#include "logger.h"

void log_this(char *message, char *file_path, pthread_mutex_t *mx) {
    pthread_mutex_lock(mx);
    FILE *fp;
    fp = fopen(file_path, "a");
    char *date = get_date_string();
    char log_entry[strlen(date) + strlen(message) + 8];
    snprintf(log_entry, sizeof(log_entry), "%s %s", date, message);
    
    fwrite(log_entry, strlen(log_entry), sizeof(char), fp);
    printf("%s\n", log_entry);
    free(date);
    fclose(fp);
    pthread_mutex_unlock(mx);
}

char *get_date_string() {
    char *current_time = malloc(128 * sizeof(char));
    time_t t;
    struct tm *ptm;
    t = time(NULL);
    ptm = localtime(&t);
    strftime(current_time, 128, "%Y-%b-%d %H:%M:%S", ptm);
    return current_time;
}