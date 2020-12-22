#ifndef __TRACE_H__
#define __TRACE_H__

#include "http_header.h"


int trace_m(char ** message, http_request_header* request_h, http_response_header* response_h);

#endif