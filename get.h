#ifndef __GET_H__
#define __GET_H__

#include "http_header.h"

int get_m(char** message, http_request_header* request_h, http_response_header* response_h, int * is_php);


#endif