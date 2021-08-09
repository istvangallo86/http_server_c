#ifndef HTTP_TOOLBOX_H
#define HTTP_TOOLBOX_H

#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <unistd.h>


// Struct to hold request data, for multithreaded processing.
struct handlerArgs {
	int socket_id;
	char request[1024];
};

char *readTextFile(char *path);
bool stringStartsWith(const char *pre, const char *str);
bool stringEndsWith(char *string, char *endswith);
void replace(char *str, char find, char replace);
void closeSocketProperly(int socket_id);
int getRequestMethod(char *request, char *method);
int getRequestURL(char *request, char *url);
void escapeJSON(char *raw_json, char *escaped_json);
void getRequestParam(char *request, char *param_name, char *value_result);
void getURLParam(char *url, char *data);
int getContentLength(char *request);
char *getImageContent(char *buffer, int content_size);
void decodeURL(char *raw_url, char *decoded_url);
void respondToBrowser(
	int socket_id,
	char *content_type,
	char *body_stream,
	int body_size);

#endif
