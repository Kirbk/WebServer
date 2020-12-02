#include "util.h"

#include <string.h>
#include <stdlib.h>
#include <ctype.h>
#include <unistd.h>

#include "http_header.h"
#include "status_code_macros.h"
#include "status_codes.h"
#include "config.h"

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

int get_resource(FILE ** goal, char * file_name, char * search_location) {
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