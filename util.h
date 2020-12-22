#ifndef __UTIL_H__
#define __UTIL_H__

#include <stdio.h>

#define TIME_TO_TIME_OUT            (5)
#define MAX_DEFAULT_FILE_INDEX      (32)

#include "http_header.h"

int get_occurrence_n(char * string, char c, int n);
int send500(int sockfd);
int send_timeout(int sockfd);
int check_permission(char* file_path);
char * get_absolute_location(char * requested);
int get_resource(char** message, char * post_data, http_request_header* request_h, http_response_header* response_h, int * is_php);
int get_error_page(char ** message, int error, http_request_header* request_h, http_response_header* response_h);

#endif