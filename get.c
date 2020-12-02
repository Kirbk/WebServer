#include "get.h"

#include <string.h>
#include <stdlib.h>
#include <ctype.h>
#include <unistd.h>

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
    *is_php = 0;

    char resource[strlen(request_h->resource) + MAX_DEFAULT_FILE_INDEX]; // Plenty of room for default file index
    memset(resource, 0, sizeof(resource));
    
    option_setting_pair* home_dir;
    if ((home_dir = get_option("HomeDir")) != NULL) {
        strcpy(resource, home_dir->settings[0]);
    } else {
        strcpy(resource, ".");
    }

    strcat(resource, request_h->resource); // Make copy to destroy with tokenizing

    char* last_i = strrchr(resource, '/');
    if (last_i == NULL || *(last_i + 1) == '\0' || strrchr(last_i, '.') == NULL) {
        if (strrchr(last_i, '.') == NULL && last_i[strlen(last_i)-1] != '/') strcat(resource, "/");
        option_setting_pair* default_index;
        if ((default_index = get_option("DefaultIndex")) != NULL) {
            strncat(resource, default_index->settings[0], MAX_DEFAULT_FILE_INDEX);
        }
    }

    int get = 0;
    char * get_string = NULL;
    if ((get = get_occurrence_n(resource, '?', 1)) != -1) {
        get_string = resource + get + 1;
        *(resource + get) = 0;
    }

    FILE* serve = fopen(resource, "rb");
    char status[20];

    if (serve == NULL) {
        get_resource(&serve, resource, request_h->referer);
    } 
    
    if (check_permission(resource) && serve != NULL) {
        fseek(serve, 0, SEEK_END);
        int size = ftell(serve);
        fseek(serve, 0, SEEK_SET);

        char* extension = strrchr(resource, '.') + 1;

        char* lwr = extension;
        for ( ; *lwr; ++lwr) *lwr = tolower(*lwr);
        lwr = NULL;

        if (strcmp(extension, "php") == 0) {
            info("Trying to run php");
            *message = run_script(resource, get_string, NULL, request_h, response_h);
            *is_php = 1;
        }

        char type_buf[32];
        get_content_type_s(type_buf, extension);

        if (strcmp(type_buf, "") == 0) strncpy(type_buf, BYTE_STREAM, sizeof(type_buf));

        char* type = malloc(strlen(type_buf) + 1);
        strcpy(type, type_buf);

        if (strcmp(response_h->status, "") == 0) {
            get_status_code_s(status, OK_S);
            response_h->status = malloc(strlen(status) + 1);
            strcpy(response_h->status, status);
        }

        response_h->connection = request_h->connection;
        if (response_h->content_length < 0) response_h->content_length = size;
        if (strcmp(response_h->content_type, "") == 0) response_h->content_type = type;

        info_v("Serving file");
        additional_v(resource);

        if (!(*is_php)) {
            *message = malloc(size);
            fread(*message, 1, size, serve);
        }
    }else if (serve == NULL) {
        // Try to find it relative to referer

        get_status_code_s(status, NOT_FOUND_S);
        response_h->status = malloc(strlen(status) + 1);
        strcpy(response_h->status, status);
        response_h->connection = request_h->connection;
        response_h->content_length = 10;
        response_h->content_type = malloc(sizeof("text/html"));
        strcpy(response_h->content_type, "text/html");

        *message = malloc(11);
        strcpy(*message, "404'd lol\n"); // Obviously supply a real (from config) 404 file

        return -1;
    } else {
        // No permission!
        info("poop");
    }

    fclose(serve);

    return 0;
}