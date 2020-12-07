#include "util.h"

#include <string.h>
#include <stdlib.h>
#include <ctype.h>
#include <unistd.h>

#include "status_code_macros.h"
#include "status_codes.h"
#include "content_type_macros.h"
#include "content_types.h"
#include "config.h"
#include "log.h"
#include "php_wrapper.h"

int get_occurrence_n(char * string, char c, int n) {
    if (string != NULL) {
        int occ = 0;
        for (int i = 0; i < strlen(string); i++) {
            if (string[i] == c) {
                if ((++occ) == n) return i;
            }
        }
    }

    return -1;
}

int send500(int sockfd) {
    char* super_error = "So much of an internal server error, I couldn't find the error page...";

    http_response_header h = create_http_response_header();
    
    http_status error = get_status_code(500);
    char error_string[50];
    sprintf(error_string, "%d %s", error.code, error.msg);

    h.connection = CLOSE;
    h.status = error_string;
    h.content_length = strlen(super_error) + 1; // Should be set to size of error page when read, setting it to temp string for now
    h.content_type = HTML;

    char* header;
    construct_response_header(&header, &h, 0);

    int c = launch_and_discard(sockfd, &header);

    // Maybe try to send error page, filename should be modifiable
    /* ... */

    write(sockfd, super_error, strlen(super_error));

    return c;
}

int send_timeout(int sockfd) {
    http_response_header h = create_http_response_header();
    char error_string[20];

    get_status_code_s(error_string, REQUEST_TIME_OUT_S);

    h.connection = CLOSE;
    h.status = error_string;

    char* header;
    construct_response_header(&header, &h, 0);

    int c = launch_and_discard(sockfd, &header);

    return c;
}

int check_permission(char* file_path) {
    return 1; // Obviously temporary, check against config
}

int get_resource_h(FILE ** goal, char * file_name, char * search_location) {
    int x = get_occurrence_n(search_location, '/', 3);
    if (x < 0) return x;

    char resource[strlen(search_location) + MAX_DEFAULT_FILE_INDEX]; // Plenty of room for default file index
    memset(resource, 0, sizeof(resource));
    
    option_setting_pair* home_dir;
    if ((home_dir = get_option("HomeDir")) != NULL) {
        strcpy(resource, home_dir->settings[0]);
    } else {
        strcpy(resource, ".");
    }

    strcat(resource, search_location + x);
    if (resource[strlen(resource)] != '/') strcat(resource, "/");
    strcat(resource, file_name);

    *goal = fopen(resource, "rb");

    return 0;
}

int get_resource(char** message, char * post_data, http_request_header* request_h, http_response_header* response_h, int * is_php) {
    if (message == NULL || request_h == NULL ||
        response_h == NULL || is_php == NULL) return INTERNAL_SERVER_ERROR_S;
         
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
        char * get_data_present = strrchr(resource, '?');
        char * get_data = NULL;
        if (get_data_present) {
            get_data = calloc(strlen(get_data_present) + 1, sizeof(char));
            strcpy(get_data, get_data_present);
            *get_data_present = '\0';
        }

        if (strrchr(last_i, '.') == NULL && last_i[strlen(last_i)-1] != '/') strcat(resource, "/");
        option_setting_pair* default_index;
        if ((default_index = get_option("DefaultIndex")) != NULL) {
            strncat(resource, default_index->settings[0], MAX_DEFAULT_FILE_INDEX);
        }

        if (get_data) {
            strcat(resource, get_data);
            free(get_data);
            get_data = 0;
        }

        get_data_present = 0;
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
        get_resource_h(&serve, resource, request_h->referer);
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
            *message = run_script(resource, get_string, post_data, request_h, response_h);
            if (*message == NULL) {
                if (response_h->status) free(response_h->status);
                get_status_code_s(status, INTERNAL_SERVER_ERROR_S);
                response_h->status = malloc(strlen(status) + 1);
                strcpy(response_h->status, status);

                response_h->connection = CLOSE;

                return INTERNAL_SERVER_ERROR_S;
            }
            *is_php = 1;
        }

        char type_buf[32];
        get_content_type_s(type_buf, extension);

        if (strcmp(type_buf, "") == 0) strncpy(type_buf, BYTE_STREAM, sizeof(type_buf));

        char* type = malloc(strlen(type_buf) + 1);
        strcpy(type, type_buf);

        if (response_h->status == NULL) {
            get_status_code_s(status, OK_S);
            response_h->status = malloc(strlen(status) + 1);
            strcpy(response_h->status, status);
        }

        response_h->connection = request_h->connection;
        if (response_h->content_length < 0) response_h->content_length = size;
        if (response_h->content_type == NULL) response_h->content_type = type;
        else if (type) free(type);

        char len[20];
        info_v("Serving file");
        additional_v(resource);
        sprintf(len, "%d", size);
        additional_v(len);

        if (!(*is_php)) {
            *message = malloc(size);
            fread(*message, 1, size, serve);
        }
    }
    else if (serve == NULL) {
        response_h->content_length = 9;
        get_status_code_s(status, NOT_FOUND_S);
        response_h->status = calloc(strlen(status) + 1, sizeof(char));
        strcpy(response_h->status, status);
        response_h->connection = request_h->connection;
        response_h->content_type = malloc(sizeof("text/html"));
        strcpy(response_h->content_type, "text/html");

        *message = calloc(10, sizeof(char));
        strcpy(*message, "404'd lol");
        return NOT_FOUND_S;
    }
    else // No permission!
        return FORBIDDEN_S;

    fclose(serve);

    return OK_S;
}