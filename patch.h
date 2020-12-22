#ifndef __PATCH_H__
#define __PATCH_H__

#include "http_header.h"


int patch_m(char ** message, http_request_header* request_h, http_response_header* response_h);

#endif