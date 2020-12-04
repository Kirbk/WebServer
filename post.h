#ifndef __POST_H__
#define __POST_H__

#include "http_header.h"

int post_m(char ** message, http_request_header* request_h, http_response_header* response_h, int * is_php);

#endif