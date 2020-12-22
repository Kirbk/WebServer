#include "delete.h"

#include <unistd.h>

#include "util.h"
#include "status_codes.h"
#include "status_code_macros.h"

int delete_m(char ** message, http_request_header* request_h, http_response_header* response_h) {
    char * resource = get_absolute_location(request_h->resource);
    int exists = access(resource, F_OK );
    int ret = 405; //exists ? 204 : 204; // Put method allowed in configuration.

    *response_h = create_http_response_header();

    ////////////////////////////////////////////////////////////////////////////////
    char * allow = "GET, POST, HEAD";
    response_h->allowed = calloc(strlen(allow) + 1, sizeof(char));
    strcpy(response_h->allowed, allow);

    get_error_page(message, 405, request_h, response_h);

    free(resource);
    return ret;
    ////////////////////////////////////////////////////////////////////////////////
        
    char status[20];
    get_status_code_s(status, ret);
    response_h->status = malloc(strlen(status) + 1);
    strcpy(response_h->status, status);

    response_h->content_location = calloc(strlen(resource) + 1, sizeof(char));
    strcpy(response_h->content_location, resource);

    remove(resource); // Maybe return page saying it was or was not deleted.

    free(resource);

    return ret;
}