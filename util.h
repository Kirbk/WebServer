#ifndef __UTIL_H__
#define __UTIL_H__

#include <stdio.h>

#define TIME_TO_TIME_OUT            (5)
#define MAX_DEFAULT_FILE_INDEX      (32)

int get_occurrence_n(char * string, char c, int n);
int send500(int sockfd);
int send_timeout(int sockfd);
int check_permission(char* file_path);
int get_resource(FILE ** goal, char * file_name, char * search_location);

#endif