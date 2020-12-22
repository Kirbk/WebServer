#include "options.h"

#include "status_codes.h"
#include "status_code_macros.h"


int options_m(char ** message, http_request_header* request_h, http_response_header* response_h) {
    *response_h = create_http_response_header();

    char * allow = "GET, POST, HEAD";
    response_h->allowed = calloc(strlen(allow) + 1, sizeof(char));
    strcpy(response_h->allowed, allow);

    char status[20];
    get_status_code_s(status, 204);
    response_h->status = malloc(strlen(status) + 1);
    strcpy(response_h->status, status);

    return 204;
}