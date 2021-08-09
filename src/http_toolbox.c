#include "http_toolbox.h"

char *readTextFile(char *path) {
	printf("INFO: Reading file \"%s\".\n", path);
	FILE *fp = fopen(path, "r");
	if (fp == NULL) {
		perror("ERROR: Error while opening a file.\n");
		char *error_text = malloc(strlen(path) + 1024);
		sprintf(error_text, "Cannot find \"%s\"", path);
		return error_text;
	}

	fseek(fp, 0, SEEK_END);
	long fsize = ftell(fp);
	fseek(fp, 0, SEEK_SET);

	char *content = malloc(fsize + 1);
	fread(content, fsize, sizeof(char), fp);
	fclose(fp);

	content[fsize] = '\0';
	return content;
}

bool stringStartsWith(const char *pre, const char *str) {
	size_t lenpre = strlen(pre), lenstr = strlen(str);
    return lenstr < lenpre ? false : strncmp(pre, str, lenpre) == 0;
}

bool stringEndsWith(char *string, char *endswith) {
	string = strrchr(string, '.');

	if( string != NULL ) {
		if(strcmp(string, endswith) == 0) {
			return true;
		}
	}

	return false;
}

void replace(char *str, char find, char replace) {
    char *current_pos = strchr(str, find);
    while(current_pos) {
        *current_pos = replace;
        current_pos = strchr(current_pos,find);
    }
}

void closeSocketProperly(int socket_id) {
	if(close(socket_id) != 0) {
		shutdown(socket_id, 2);	
	}
}

int getRequestMethod(char *request, char *method) {
	if(stringStartsWith("GET", request) == true) {
		sprintf(method, "%s", "GET");
		return 0;
	} else if(stringStartsWith("POST", request) == true) {
		sprintf(method, "%s", "POST");
		return 0;
	}

	sprintf(method, "%s", "ERROR");
	return -1;
}

int getRequestURL(char *request, char *url) {
	int i = 0;
	int j = 0;
	int collect_url = 0;
	char collected_url[1024];

	for(i = 0; i < strlen(request); i++) {
		if(request[i] == '/') {
			collect_url = 1;
		}

		if((request[i] == ' ' || request[i] == '\t') && collect_url == 1) {
			collect_url = 0;
			collected_url[j] = '\0';
			sprintf(url, "%s", collected_url);
			return 0;
		}

		if(collect_url == 1) {
			collected_url[j] = request[i];
			j++;
		}
	}

	return -1;
}

// TODO: Error handling for this function.
void getRequestParam(char *request, char *param_name, char *value_result) {
	char *result = strstr(request, param_name);
	int position = result - request;
	int substringLength = strlen(request) - position;
	int i, j = 0;
	int start_collect = 0;
	for(i = 0; i <= substringLength; i++) {
		if(request[position] == '&' || request[position] == ' ') {
			value_result[j] = '\0';
			break;
		}

		// Decode the HTTP encoding.
		if(start_collect == 1) {
			if(request[position] == '+') {
				value_result[j] = ' ';
			} else if(
				request[position] == '%' && 
				request[position + 1] == '2' && 
				request[position + 2] == '0') {
				
				value_result[j] = ' ';
				position += 2;
			} else if(
				request[position] == '%' && 
				request[position + 1] == '2' && 
				request[position + 2] == '1') {
				
				value_result[j] = '!';
				position += 2;
			} else if(
				request[position] == '%' && 
				request[position + 1] == '2' && 
				request[position + 2] == '2') {

				value_result[j] = '\"';
				position += 2;
			} else if(
				request[position] == '%' && 
				request[position + 1] == '2' && 
				request[position + 2] == '3') {
				
				value_result[j] = '#';
				position += 2;
			} else if(
				request[position] == '%' && 
				request[position + 1] == '2' &&
				request[position + 2] == '4') {

				value_result[j] = '$';
				position += 2;
			} else if(
				request[position] == '%' && 
				request[position + 1] == '2' && 
				request[position + 2] == '5') {
				
				value_result[j] = '%';
				position += 2;
			} else if(
				request[position] == '%' && 
				request[position + 1] == '2' && 
				request[position + 2] == '6') {

				value_result[j] = '&';
				position += 2;
			} else if(
				request[position] == '%' && 
				request[position + 1] == '2' && 
				request[position + 2] == '7') {

				value_result[j] = '\'';
				position += 2;
			} else if(
				request[position] == '%' && 
				request[position + 1] == '2' && 
				request[position + 2] == 'A') {

				value_result[j] = '*';
				position += 2;
			} else if(
				request[position] == '%' &&
				request[position + 1] == '2' &&
				request[position + 2] == 'B') {

				value_result[j] = '+';
				position += 2;
			} else if(
				request[position] == '%' &&
				request[position + 1] == '2' &&
				request[position + 2] == 'C') {

				value_result[j] = ',';
				position += 2;
			} else if(
				request[position] == '%' &&
				request[position + 1] == '2' &&
				request[position + 2] == 'D') {

				value_result[j] = '-';
				position += 2;
			} else if(
				request[position] == '%' &&
				request[position + 1] == '2' &&
				request[position + 2] == 'E') {
				
				value_result[j] = '.';
				position += 2;
			} else if(
				request[position] == '%' &&
				request[position + 1] == '2' &&
				request[position + 2] == 'F') {

				value_result[j] = '/';
				position += 2;
			} else if(
				request[position] == '%' &&
				request[position + 1] == '5' &&
				request[position + 2] == 'C') {

				value_result[j] = '\\';
				position += 2;
			} else if(
				request[position] == '%' &&
				request[position + 1] == 'C' &&
				request[position + 2] == '2' &&
				request[position + 3] == '%' &&
				request[position + 4] == 'B' &&
				request[position + 5] == '2') {
				value_result[j] = '2';
				position += 5;
			} else if(
				request[position] == '%' && 
				request[position + 1] == '4' && 
				request[position + 2] == '0') {

				value_result[j] = '@';
				position += 2;
			} else {
				value_result[j] = request[position];
			}

			j++;
		}

		if(request[position] == '=') {
			start_collect = 1;
		}

		position++;
	}
}

void getURLParam(char *url, char *data) {
	int slashes = 0;
	int i, j = 0;
	for(i = 0; i < strlen(url); i++) {
		if(slashes == 2) {
			data[j] = url[i];
			j++;
		}

		if(url[i] == '/') {
			slashes++;
		}
	}

	data[j] = '\0';
}

void decodeURL(char *raw_url, char *decoded_url) {
	int i;
	int j = 0;
	for(i = 0; i < strlen(raw_url); i++) {
		// Decode the HTTP encoding.
		if(
			raw_url[i] == '%' &&
			raw_url[i + 1] == '2' &&
			raw_url[i + 2] == '0') {

			decoded_url[j] = ' ';
			i += 2;
		} else if(
			raw_url[i] == '%' &&
			raw_url[i + 1] == '2' &&
			raw_url[i + 2] == '1') {

			decoded_url[j] = '!';
			i += 2;
		} else if(
			raw_url[i] == '%' &&
			raw_url[i + 1] == '2' &&
			raw_url[i + 2] == '2') {

			decoded_url[j] = '\"';
			i += 2;
		} else if(
			raw_url[i] == '%' &&
			raw_url[i + 1] == '2' &&
			raw_url[i + 2] == '3') {

			decoded_url[j] = '#';
			i += 2;
		} else if(
			raw_url[i] == '%' &&
			raw_url[i + 1] == '2' &&
			raw_url[i + 2] == '4') {

			decoded_url[j] = '$';
			i += 2;
		} else if(
			raw_url[i] == '%' &&
			raw_url[i + 1] == '2' &&
			raw_url[i + 2] == '5') {

			decoded_url[j] = '%';
			i += 2;
		} else if(
			raw_url[i] == '%' &&
			raw_url[i + 1] == '2' &&
			raw_url[i + 2] == '6') {

			decoded_url[j] = '&';
			i += 2;
		} else if(
			raw_url[i] == '%' &&
			raw_url[i + 1] == '2' &&
			raw_url[i + 2] == '7') {

			decoded_url[j] = '\'';
			i += 2;
		} else if(
			raw_url[i] == '%' &&
			raw_url[i + 1] == '2' &&
			raw_url[i + 2] == 'A') {

			decoded_url[j] = '*';
			i += 2;
		} else if(
			raw_url[i] == '%' &&
			raw_url[i + 1] == '2' &&
			raw_url[i + 2] == 'B') {

			decoded_url[j] = '+';
			i += 2;
		} else if(
			raw_url[i] == '%' &&
			raw_url[i + 1] == '2' &&
			raw_url[i + 2] == 'C') {

			decoded_url[j] = ',';
			i += 2;
		} else if(
			raw_url[i] == '%' &&
			raw_url[i + 1] == '2' &&
			raw_url[i + 2] == 'D') {

			decoded_url[j] = '-';
			i += 2;
		} else if(
			raw_url[i] == '%' && 
			raw_url[i + 1] == '2' && 
			raw_url[i + 2] == 'E') {

			decoded_url[j] = '.';
			i += 2;
		} else if(
			raw_url[i] == '%' && 
			raw_url[i + 1] == '4' && 
			raw_url[i + 2] == '0') {

			decoded_url[j] = '@';
			i += 2;
		} else {
			decoded_url[j] = raw_url[i];
		}

		j++;
	}

	decoded_url[j] = '\0';
}

void escapeJSON(char *raw_json, char *escaped_json) {
	int i, j = 0;

	for(i = 0; i < strlen(raw_json); i++) {
		switch (raw_json[i]) {
			case '\b':
					escaped_json[j] = '\\';
					j++;
					escaped_json[j] = 'b';
					break;
			case '\f':
					escaped_json[j] = '\\';
					j++;
					escaped_json[j] = 'f';
					break;
			case '\n':
					escaped_json[j] = '\\';
					j++;
					escaped_json[j] = 'n';
					break;
			case '\r':
					escaped_json[j] = '\\';
					j++;
					escaped_json[j] = 'r';
					break;
			case '\t':
					escaped_json[j] = '\\';
					j++;
					escaped_json[j] = 't';
					break;
			// INFO: Double quotes don't has to be escaped, as they already
			//       escaped in the database handlers.
			/*case '\"':
					escaped_json[j] = '\\';
					j++;
					escaped_json[j] = '\"';
					break;*/
			case '\\':
					escaped_json[j] = '\\';
					j++;
					escaped_json[j] = '\\';
					break;
			case '/':
					escaped_json[j] = '\\';
					j++;
					escaped_json[j] = '/';
					break;
			default: escaped_json[j] = raw_json[i];
		}

		j++;
	}
}

int getContentLength(char *buffer) {
	char *substring = "Content-Length: ";
	char *ptr;
	ptr = strstr(buffer, substring);
	int i, j;

	char *length = malloc(sizeof(char) * 32);
	j = 0;
	for(i = strlen(substring); i < strlen(ptr); i++) {
		if(ptr[i] == '\n') {
			break;
		}

		length[j] = ptr[i];
		j++;
	}

	length[j] = '\0';
	int content_length = atoi(length);
	free(length);
	return content_length;
}

char *getImageContent(char *buffer, int content_size) {
	char *substring = "Content-Type: image/png\r\n\r\n";
	char *ptr;
	ptr = strstr(buffer, substring);
	int i, j;

	char *content = malloc(sizeof(char) * (content_size + 1));
	j = 0;
	for(i = strlen(substring); i < content_size; i++) {
		content[j] = ptr[i];
		j++;
	}

	content[j] = '\0';
	return content;
}

void respondToBrowser(
	int socket_id,
	char *content_type,
	char *body_stream,
	int body_size) {
	
	char *response = malloc(body_size + 2048);
	sprintf(response,
		"HTTP/1.1 200 OK\n"
		"Content-length: %d\n"
		"Content-Type: %s\n\n%s\n",
		body_size, content_type, body_stream);

	write(socket_id, response, strlen(response));
	free(response);
    closeSocketProperly(socket_id);
}
