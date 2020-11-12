#include "client.h"

#include <unistd.h>
#include <string.h>
#include <sys/select.h>
#include <ctype.h>
#include <time.h>

#include "config.h"
#include "log.h"
#include "http_header.h"
#include "status_codes.h"
#include "content_types.h"
#include "status_code_macros.h"
#include "content_type_macros.h"

#define TIME_TO_TIME_OUT            (5)
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

    h.connection = CLOSE;
    h.status = error_string;

    char* header;
    construct_response_header(&header, &h);

    int c = launch_and_discard(sockfd, &header);

    return c;
}

int check_permission(char* file_path) {
    return 1; // Obviously temporary, check against config
}

int get_m(char** message, http_request_header* request_h, http_response_header* response_h) {
    // *message = calloc(5, sizeof(char));
    // strcpy(*message, "FUCK");
    // info(*message);

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

    FILE* serve;
    char status[20];
    if (check_permission(resource) && (serve = fopen(resource, "rb")) != NULL) {
        fseek(serve, 0, SEEK_END);
        int size = ftell(serve);
        fseek(serve, 0, SEEK_SET);

        char* extension = strrchr(resource, '.') + 1;

        char* lwr = extension;
        for ( ; *lwr; ++lwr) *lwr = tolower(*lwr);
        lwr = NULL;

        char type_buf[32];
        get_content_type_s(type_buf, extension);

        if (strcmp(type_buf, "") == 0) strncpy(type_buf, BYTE_STREAM, sizeof(type_buf));

        char* type = malloc(strlen(type_buf) + 1);
        strcpy(type, type_buf);

        get_status_code_s(status, OK_S);
        response_h->status = malloc(strlen(status) + 1);
        strcpy(response_h->status, status);
        response_h->connection = request_h->connection;
        response_h->content_length = size;
        response_h->content_type = type;

        info_v("Serving file");
        additional_v(resource);

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
        info("poop");
    }

    fclose(serve);

    return 0;
}

int post_m(http_request_header* request_h, http_response_header* response_h) {
    return -1;
}

int dispatch(int sockfd, char* clientip) {
    int loop = 1;
    char* reason;

    FILE* log_file;

    while (loop) {
        char buffer[MAX_HEADER_LENGTH];
        memset(buffer, 0, MAX_HEADER_LENGTH);

        fd_set read_fds, write_fds, except_fds;
        FD_ZERO(&read_fds);
        FD_ZERO(&write_fds);
        FD_ZERO(&except_fds);
        FD_SET(sockfd, &read_fds);

        // Set timeout to 1.0 seconds
        struct timeval timeout;
        timeout.tv_sec = TIME_TO_TIME_OUT;
        timeout.tv_usec = 0;

        if (select(sockfd + 1, &read_fds, &write_fds, &except_fds, &timeout) != 1) {
            send_timeout(sockfd);
            reason = "Time Out";
            break;
        }

        int bytes_read = read(sockfd, buffer, sizeof(buffer));
        if (bytes_read > MAX_HEADER_LENGTH) {
            // Header too big!
        } else if (bytes_read == 0) continue;

        log_file = fopen("log.txt", "a");
        fwrite(buffer, strlen(buffer), 1, log_file);
        fwrite("----------------------------------------------------------------------------------------------------\n", 101, 1, log_file);
        fclose(log_file);
        http_request_header* request = parse_request_header(buffer);

        if (request->connection == CLOSE) {
            reason = "Client Requested";
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
            info("POST");
            break;
        case PUT:
            info("PUT");
            break;
        case DELETE:
            info("DELETE");
            break;
        case CONNECT:
            info("CONNECT");
            break;
        case OPTIONS:
            info("OPTIONS");
            break;
        case TRACE:
            info("TRACE");
            break;
        case PATCH:
            info("PATCH");
            break;
        case INVALID:
            send500(sockfd);
            break;
        }

        char date[50];
        time_t now = time(0);
        struct tm tm = *gmtime(&now);
        strftime(date, sizeof date, "%a, %d %b %Y %H:%M:%S %Z", &tm);
        response.date = date;
        response.keep_alive.timeout = TIME_TO_TIME_OUT;

        char* header_text;
        if (construct_response_header(&header_text, &response) < 0) {
            send500(sockfd);
            reason = "Server Error";
            break;
        }

        launch_and_discard(sockfd, &header_text);
        free_request_header(&request);

        if (message) {
            if (!head_set) write(sockfd, message, (response.content_length != 0) ? response.content_length : (strlen(message) + 1));
            free(message);
        }
    }

    info_v("Client Disconnected!");
    additional_v(clientip);
    additional_v(reason);

    close(sockfd);
    exit(0);
}
