#include "trace.h"

#include "util.h"

int trace_m(char ** message, http_request_header* request_h, http_response_header* response_h) {
    *response_h = create_http_response_header();

    char * allow = "GET, POST, HEAD";
    response_h->allowed = calloc(strlen(allow) + 1, sizeof(char));
    strcpy(response_h->allowed, allow);

    get_error_page(message, 405, request_h, response_h);

    return 405;
}