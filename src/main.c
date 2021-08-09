// VERSION 0.2

#include <stdio.h>
#include <stdbool.h>
#include <netinet/in.h>
#include <netdb.h>
#include <sys/socket.h>
#include <sys/stat.h>	
#include <sys/types.h>
#include <unistd.h>
#include <pthread.h>
#include <time.h>

#include "http_toolbox.h"
#include "logger.h"


#define LOG_FILE_NAME "log.txt"

// Set some global valuables.
int REQUEST_BUFFER_SIZE = 4056;

// A global mutex to lock, to handle the paralel access to logging
pthread_mutex_t mutex_lock_logger;

// Request handler. Each request is processed in a separate thread. 
void *async_request_handler(void *args) {
	struct handlerArgs local_args = *(struct handlerArgs*)args;
	free(args);
	char method[10];
	char url[2048];

	if(getRequestMethod(local_args.request, method) < 0) {
		log_this(
			"ERROR: Could not dig out HTTP method from request.\n",
			LOG_FILE_NAME,
			&mutex_lock_logger);

		closeSocketProperly(local_args.socket_id);
		return NULL;
	};

	if(getRequestURL(local_args.request, url) < 0) {
		log_this(
			"ERROR: Could not dig out URL from request.\n",
			LOG_FILE_NAME,
			&mutex_lock_logger);

		closeSocketProperly(local_args.socket_id);		
		return NULL;
	}

	char *log = "INFO: New request arrived -> %s %s\n";
	char message[sizeof(log) + sizeof(method) + sizeof(url) + 1];
	snprintf(
		message,
		sizeof(message),
		log,
		method,
		url);

	log_this(message, LOG_FILE_NAME, &mutex_lock_logger);
	
	// Dealing with the handlers.
	if(
		strcmp(method, "GET") == 0 &&
		strcmp(url, "/") == 0) {

		char *body_stream = readTextFile("web/index.htm");
		respondToBrowser(
			local_args.socket_id,
			"text/html",
			body_stream,
			strlen(body_stream));

		// After "readTextFile" the body stream has to be freed!"
		free(body_stream);

	} else if(
		strcmp(method, "GET") == 0 &&
		stringStartsWith("/public/js/", url) == true) {

		char file_path[512];
		if(snprintf(file_path, sizeof(file_path), "web%s", url) < 0) {
			log_this(
				"ERROR: Requested public file path too long.\n",
				LOG_FILE_NAME,
				&mutex_lock_logger);
		}

		char *body_stream = readTextFile(file_path);
		respondToBrowser(
			local_args.socket_id,
			"text/javascript",
			body_stream,
			strlen(body_stream));

		// After "readTextFile" the body stream has to be freed!"
		free(body_stream);

	} else if(
		strcmp(method, "GET") == 0 &&
		stringStartsWith("/public/css/", url) == true) {

		char file_path[512];
		if(snprintf(file_path, sizeof(file_path), "web%s", url) < 0) {
			log_this(
				"ERROR: Requested public file path too long.\n",
				LOG_FILE_NAME,
				&mutex_lock_logger);
		}

		char *body_stream = readTextFile(file_path);
		respondToBrowser(
			local_args.socket_id,
			"text/css",
			body_stream,
			strlen(body_stream));

		// After "readTextFile" the body stream has to be freed!"
		free(body_stream);

	} else if(
		strcmp(method, "GET") == 0 &&
		(stringStartsWith("/public/img/", url) == true ||
		stringStartsWith("/public/images/", url) == true)) {

		char file_path[512];
		if(snprintf(file_path, sizeof(file_path),  "web%s", url) < 0) {
			log_this(
				"ERROR: Requested public file path too long.\n",
				LOG_FILE_NAME,
				&mutex_lock_logger);
		}

		int file_size;
		FILE *fp;
		fp = fopen(file_path, "rb");
		if(fp == NULL) {
			perror("ERROR: Cannot open png file.\n");
			closeSocketProperly(local_args.socket_id);
			return 0;
		}

		fseek(fp, 0, SEEK_END);
		file_size = ftell(fp);
		fseek(fp, 0, SEEK_SET);

		char png_file[file_size];
		fread(png_file, file_size, sizeof(char), fp);
		fclose(fp);
		respondToBrowser(local_args.socket_id, "image/png", png_file, file_size);

	} else {
		char *body_stream = readTextFile("web/error_404.htm");
		respondToBrowser(
			local_args.socket_id,
			"text/html",
			body_stream,
			strlen(body_stream));

		// After "readTextFile" the body stream has to be freed!"
		free(body_stream);
	}

	return NULL;
}

int main() {
	int create_socket;
	int new_socket;
	int server_port = 8000;
	socklen_t client_len;
	struct sockaddr_in server, client;
	client_len = sizeof(client);
	
	// Initialise the mutex lock.
	if(pthread_mutex_init(&mutex_lock_logger, NULL) != 0) { 
		return 1; 
	}

	log_this(
		"INFO: Creating a new socket.\n",
		LOG_FILE_NAME,
		&mutex_lock_logger);

	create_socket = socket(AF_INET, SOCK_STREAM, 0);
	int opt = 1;
	setsockopt(create_socket, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));
	setsockopt(create_socket, SOL_SOCKET, SO_KEEPALIVE, &opt, sizeof(opt));
	
	if(create_socket > 0) {
		log_this(
			"INFO: Socket created successfully.\n",
			LOG_FILE_NAME,
			&mutex_lock_logger);
	} else {
		log_this(
			"ERROR: Could not create new socket.",
			LOG_FILE_NAME,
			&mutex_lock_logger);
		exit(0);
	}
	
	bzero((char *)&server, sizeof(server));
	server.sin_family = AF_INET;
	server.sin_addr.s_addr = INADDR_ANY;
	server.sin_port = htons(server_port);
	
	int bind_value = -1;
	while(bind_value < 0) {
		log_this("INFO: Binding socket.\n", LOG_FILE_NAME, &mutex_lock_logger);
		bind_value = bind(
			create_socket,
			(struct sockaddr *)&server,
			sizeof(server));
			
		sleep(1);
	}
	
	log_this(
		"INFO: Socket successfully binded.\n",
		LOG_FILE_NAME,
		&mutex_lock_logger);

	listen(create_socket, 1024);
	
	ssize_t request_string_size;
	
	while(1) {
		char buffer[REQUEST_BUFFER_SIZE];

		new_socket = accept(
			create_socket,
			(struct sockaddr*)&client,
			&client_len);

		if(new_socket < 0) {
			log_this(
				"ERROR: Server couldn't accept the request. Skipping to "
				"next loop.\n",
				LOG_FILE_NAME,
				&mutex_lock_logger);
				
			continue;
		} else {
			log_this(
				"INFO: A new client has been connected.\n",
				LOG_FILE_NAME,
				&mutex_lock_logger);
		}
		
		request_string_size = recv(new_socket, buffer, REQUEST_BUFFER_SIZE, 0);
		if(request_string_size <= 0) {
			log_this(
				"WARNING: Unidentified request. The request size invalid.\n",
				LOG_FILE_NAME,
				&mutex_lock_logger);
				
			log_this(
				"INFO: Skipping loop.\n",
				LOG_FILE_NAME,
				&mutex_lock_logger);

			closeSocketProperly(new_socket);
			continue;
		}

		// Put each request into a separate thread. Allocate memory for 
		// "struct handlerArgs" before leave the scope with the new thread.
		// The allocation must be freed up in the threaded function.
		struct handlerArgs *args = malloc(sizeof(struct handlerArgs));
		args->socket_id = new_socket;
		strcpy(args->request, buffer);
		pthread_t new_thread;
		pthread_create(&new_thread, NULL, async_request_handler, (void *)args);
		pthread_detach(new_thread);
		
		//sleep(0.01); //-> This is optional, not a must.
	}
	
	// Clean up.
	close(create_socket);
	pthread_mutex_destroy(&mutex_lock_logger);

	return 0;
}
