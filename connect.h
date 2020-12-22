#ifndef __CONNECT_H__
#define __CONNECT_H__

#include "http_header.h"


int connect_m(char ** message, http_request_header* request_h, http_response_header* response_h);


#endif