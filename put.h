#ifndef __PUT_H__
#define __PUT_H__

#include "http_header.h"

int put_m(char ** message, http_request_header* request_h, http_response_header* response_h);

#endif