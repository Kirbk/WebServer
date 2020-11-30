#ifndef __PHP_WRAPPER_H__
#define __PHP_WRAPPER_H__

#include "http_header.h"


#define die(e) do { fprintf(stderr, "%s\n", e); exit(EXIT_FAILURE); } while (0);

char * run_script(char * file_name, char * q_string, http_request_header * header);


#endif