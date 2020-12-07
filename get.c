#include "get.h"

#include <string.h>
#include <stdlib.h>
#include <ctype.h>
#include <unistd.h>

#include <assert.h>

#include "util.h"
#include "config.h"
#include "log.h"
#include "http_header.h"
#include "content_types.h"
#include "content_type_macros.h"
#include "status_code_macros.h"
#include "status_codes.h"
#include "php_wrapper.h"

int get_m(char** message, http_request_header* request_h, http_response_header* response_h, int * is_php) {
    int ret = get_resource(message, NULL, request_h, response_h, is_php);

    if (ret == INTERNAL_SERVER_ERROR_S) {
        // *message === error page from config.
        return -1;
    }

    int first = ret;
    while(first >= 10)
    {
        first = first / 10;
    }

    if (first > 3) {
        if (message && *message != NULL) *message = NULL;
        if (response_h) {
            free_response_header(response_h);
            *response_h = create_http_response_header();
        }
        get_error_page(message, ret, request_h, response_h);
    }

    return 0;
}