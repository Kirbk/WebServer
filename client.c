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
    int sockets[2];
    if (pipe(sockets) < 0) {
        info("Client dropped due to a server error!");
        system_out();
        perror("");

        send500(sockfd);
        close(sockfd);
        return(-1);
    }


    if (fork()) { // Writer
        int loop = 1;
        char fork_buf[1024];
        close(sockets[1]);
        while (read(sockets[0], fork_buf, sizeof(fork_buf)) > 0);
        printf("%s\n", fork_buf);
        if (strcmp(fork_buf, "NO_LOOP") == 0) loop = 0;

        while (loop) {
            // Respond and keep alive, maybe change to a do-while loop
        }
    } else { // Reader
        int loop = 1;

        while (loop) {
            close(sockets[0]);
            char buffer[2048];
            read(sockfd, buffer, sizeof(buffer));

            http_request_header* received = parse_request_header(buffer);

            if (received->connection == CLOSE) {
                write(sockets[1], "NO_LOOP", sizeof("NO_LOOP"));
                info("fuck");
                loop = 0;
            }
        }
    }

    info("Client Disconnected!");
    additional(clientip);

    close(sockfd);
    exit(0);
}