#include "post.h"

#include "util.h"

int post_m(char ** message, http_request_header * request_h, http_response_header* response_h, int * is_php) {
    int ret = get_resource(message, request_h->message, request_h, response_h, is_php);
}