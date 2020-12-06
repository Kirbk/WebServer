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
#include "php_wrapper.h"
#include "util.h"
#include "get.h"
#include "post.h"

int dispatch(int sockfd, char* clientip) {
    int loop = 1;
    int php = 0;
    char* reason;

    FILE* log_file;

    while (loop) {
        char buffer[MAX_HEADER_LENGTH] = { 0 };

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
        
        char * req_header_text = NULL;
        int bytes_read = 0;
        int ret_size = 0;
        
        int x = 0;

        int buf_ind = 0;
        int cont = 1;
        while (cont) {
            while (buf_ind < MAX_HEADER_LENGTH && 1 == read(sockfd, &buffer[buf_ind], 1)) {
                if (buf_ind > 2                 && 
                    '\n' == buffer[buf_ind]     &&
                    '\r' == buffer[buf_ind - 1] &&
                    '\n' == buffer[buf_ind - 2] &&
                    '\r' == buffer[buf_ind - 3])
                {
                    cont = 0;
                    break;
                }
                buf_ind++;
            }

            buf_ind++;

            printf("%d\n", buf_ind);
            printf("%ld\n", strlen(buffer));

            printf("%s\n", buffer);

            if (req_header_text == NULL) {
                req_header_text = (char*)malloc(buf_ind * sizeof(char) + 1);
                memset(req_header_text, 0, buf_ind + 1);
                strncpy(req_header_text, buffer, buf_ind);

                ret_size = buf_ind + 1;
            } else {
                req_header_text = (char*) realloc(req_header_text, (ret_size += buf_ind));
                memset(req_header_text + ret_size - 1, 0, 1);
                strncat(req_header_text, buffer, buf_ind);
            }



            memset(buffer, 0, MAX_HEADER_LENGTH);
        }

        // while ((bytes_read = read(sockfd, buffer, sizeof(buffer))) > 0) {
        //     printf("%d\n", bytes_read);
        //     if (req_header_text == NULL) {
        //         req_header_text = (char*)malloc(bytes_read * sizeof(char) + 1);
        //         memset(req_header_text, 0, bytes_read + 1);
        //         strncpy(req_header_text, buffer, bytes_read);

        //         ret_size = bytes_read + 1;
        //     } else {
        //         req_header_text = (char*) realloc(req_header_text, (ret_size += bytes_read));
        //         memset(req_header_text + ret_size - 1, 0, 1);
        //         strncat(req_header_text, buffer, bytes_read);
        //     }



        //     memset(buffer, 0, MAX_HEADER_LENGTH);


        //     // if (bytes_read < MAX_HEADER_LENGTH) break;
        // }

        log_file = fopen("log.txt", "a");
        fwrite("\n----------------------------------------------------------------------------------------------------\n", 102, 1, log_file);
        fwrite(req_header_text, strlen(req_header_text), 1, log_file);
        fwrite("\n\n", 2, 1, log_file);
        http_request_header* request = parse_request_header(req_header_text);

        free(req_header_text);

        if (request->connection == CLOSE) {
            reason = "Client Requested";
            loop = 0;
        }

        // Create response
        http_response_header response = create_http_response_header();
        response.connection = request->connection;

        char* message = 0; // malloc the message then delete it.
        int head_set = 0;

        switch(request->method) {
        case HEAD:
            head_set = 1;
        case GET:
            get_m(&message, request, &response, &php);
            break;
        case POST:
            post_m(&message, request, &response, &php);
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
        response.date = calloc(strlen(date) + 1, sizeof(char));
        strcpy(response.date, date);
        response.keep_alive.timeout = TIME_TO_TIME_OUT;

        char* header_text;
        if (construct_response_header(&header_text, &response, php) < 0) {
            send500(sockfd);
            reason = "Server Error";
            break;
        }

        free_response_header(&response);

        fwrite(header_text, strlen(header_text), 1, log_file);
        fwrite("----------------------------------------------------------------------------------------------------\n", 101, 1, log_file);
        fclose(log_file);

        launch_and_discard(sockfd, &header_text);
        free_request_header(&request);

        if (message) {
            if (!head_set) write(sockfd, message, (response.content_length != 0 && !php) ? response.content_length : (strlen(message) + 1));
            free(message);
        }
    }

    info_v("Client Disconnected!");
    additional_v(clientip);
    additional_v(reason);

    close(sockfd);
    exit(0);
}
