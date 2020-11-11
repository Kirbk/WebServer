#include "client.h"

#include <unistd.h>
#include <string.h>
#include <sys/select.h>

#include "config.h"
#include "log.h"
#include "http_header.h"
#include "status_codes.h"
#include "status_code_macros.h"

#define TIME_TO_TIME_OUT            (30)
#define MAX_DEFAULT_FILE_INDEX      (32)

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
    construct_response_header(&header, &h);

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
    printf("%s\n", error_string);

    return 0; // NOT DONE IDIOT
}

int check_permission(char* file_path) {
    return 1; // Obviously temporary, check against config
}

int get_m(char** message, http_request_header* request_h, http_response_header* response_h) {
    // *message = calloc(5, sizeof(char));
    // strcpy(*message, "FUCK");
    // info(*message);

    char resource[sizeof(request_h->resource) + MAX_DEFAULT_FILE_INDEX]; // Plenty of room for default file index
    memset(resource, 0, sizeof(resource));
    strcpy(resource, ".");
    strcat(resource, request_h->resource); // Make copy to destroy with tokenizing

    char* last_i = strrchr(resource, '/');
    if (last_i == NULL || *(last_i + 1) == '\0' || strrchr(last_i, '.') == NULL) {
        if (strrchr(last_i, '.') == NULL && last_i[strlen(last_i)-1] != '/') strcat(resource, "/");
        option_setting_pair* default_index;
        if ((default_index = get_option("DefaultIndex")) != NULL) {
            strncat(resource, default_index->settings[0], MAX_DEFAULT_FILE_INDEX);
        }
    }

    info(resource);

    FILE* serve;
    char status[20];
    if (check_permission(resource) && (serve = fopen(resource, "rb")) != NULL) {
        fseek(serve, 0, SEEK_END);
        int size = ftell(serve);
        fseek(serve, 0, SEEK_SET);

        get_status_code_s(status, OK_S);
        response_h->status = malloc(strlen(status) + 1);
        strcpy(response_h->status, status);
        response_h->connection = request_h->connection;
        response_h->content_length = size;
        // response_h->content_type = ;

        *message = malloc(size);
        fread(*message, 1, size, serve);
    } else if (serve == NULL) {
        get_status_code_s(status, NOT_FOUND_S);
        response_h->status = malloc(strlen(status) + 1);
        strcpy(response_h->status, status);
        response_h->connection = request_h->connection;
        response_h->content_length = 11;
        response_h->content_type = malloc(sizeof("text/html"));
        strcpy(response_h->content_type, "text/html");

        *message = malloc(10);
        strcpy(*message, "404'd lol\n"); // Obviously supply a real (from config) 404 file

        return -1;
    } else {
        // No permission!
    }

    return 0;
}

int post_m(http_request_header* request_h, http_response_header* response_h) {

}

int dispatch(int sockfd, char* clientip) {
    int loop = 1;

    while (loop) {
        char buffer[2048];

        fd_set read_fds, write_fds, except_fds;
        FD_ZERO(&read_fds);
        FD_ZERO(&write_fds);
        FD_ZERO(&except_fds);
        FD_SET(sockfd, &read_fds);

        // Set timeout to 1.0 seconds
        struct timeval timeout;
        timeout.tv_sec = TIME_TO_TIME_OUT;
        timeout.tv_usec = 0;

        if (select(sockfd + 1, &read_fds, &write_fds, &except_fds, &timeout) != 1) { // This is stupid, should not kill the connection, wrong use of time out
            if (send_timeout(sockfd) == -1) {
                break;
            }
        }

        read(sockfd, buffer, sizeof(buffer));

        http_request_header* request = parse_request_header(buffer);

        if (request->connection == CLOSE) {
            loop = 0;
        }

        // Create response
        http_response_header response = create_http_response_header();
        response.connection = request->connection;

        char* message; // malloc the message then delete it.
        int head_set = 0;

        switch(request->method) {
        case HEAD:
            head_set = 1;
        case GET:
            get_m(&message, request, &response);
            break;
        case POST:

            break;
        case PUT:

            break;
        case DELETE:

            break;
        case CONNECT:

            break;
        case OPTIONS:

            break;
        case TRACE:

            break;
        case PATCH:

            break;
        }

        char* header_text;
        if (construct_response_header(&header_text, &response) < 0) {
            send500(sockfd);
            break;
        }

        info(response.status);
        info(header_text);

        launch_and_discard(sockfd, &header_text);
        free_request_header(&request);

        if (message) {
            if (!head_set) write(sockfd, message, strlen(message) + 1);
            free(message);
        }
    }

    info("Client Disconnected!");
    additional(clientip);

    close(sockfd);
    exit(0);
}