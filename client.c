#include "client.h"

#include <unistd.h>
#include <string.h>

#include "log.h"
#include "http_header.h"
#include "status_codes.h"

void send500(int sockfd) {
    http_response_header h = create_http_response_header();
    
    http_status error = get_status_code(500);
    char error_string[50];
    sprintf(error_string, "%d %s", error.code, error.msg);

    h.connection = CLOSE;
    h.status = error_string;

    char* header;
    construct_response_header(&header, &h);
    launch_and_discard(sockfd, &header);
}

int dispatch(int sockfd, char* clientip) {
    int loop = 1;

    while (loop) {
        char buffer[2048];
        read(sockfd, buffer, sizeof(buffer));

        http_request_header* received = parse_request_header(buffer);

        if (received->connection == CLOSE) {
            loop = 0;
        }

        // Create response
        http_response_header response = create_http_response_header();
        response.connection = received->connection;

        switch(received->method) {
        case GET:
            response.status = "200 OK";
            break;
        case HEAD:

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
        construct_response_header(&header_text, &response); // Test to make sure it actually created it

        launch_and_discard(sockfd, &header_text); // Not sure if it is sending

        free_request_header(&received); // Causing malloc error
    }

    info("Client Disconnected!");
    additional(clientip);

    close(sockfd);
    exit(0);
}