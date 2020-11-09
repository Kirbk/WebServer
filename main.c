#include <stdio.h> 
#include <netdb.h> 
#include <netinet/in.h> 
#include <stdlib.h> 
#include <string.h> 
#include <unistd.h>
#include <sys/socket.h> 
#include <sys/types.h> 
#include <signal.h>
#define MAX 512
#define MAX_FILE_PATH 512
#define PORT 80
#define SA struct sockaddr 

typedef struct {
    char* status;
    char* connection;
    char* content_type;
    unsigned int content_length;
} header_content;


void craft_header(char** header, char* code, unsigned int length) {
    char length_string[100];
    sprintf(length_string, "%d", length);

    char head[MAX] = "HTTP/1.1 ";
    strcat(head, code);
    strcat(head, "\r\nContent-Length: ");
    strcat(head, length_string);
    strcat(head, "\r\nConnection: close\r\n\n");

    printf("%s\n", head);

    *header = head;
}

// Function designed for chat between client and server. 
void func(int sockfd) 
{
	char buff[MAX]; 
	int n; 
	// infinite loop for chat 
    bzero(buff, MAX); 

    // read the message from client and copy it in buffer 
    read(sockfd, buff, sizeof(buff)); 
    // print buffer which contains the client contents 

    // Process Request
    char* loc;
    if ((loc = strstr(buff, "GET")) == NULL) {
        fprintf(stderr, "Can't find \"GET\" in request!\n");
        close(sockfd);
        exit(-1);
    }
    const char delimiter[2] = " ";
    char serve_file_path[MAX_FILE_PATH] = "./TestWeb";

    strtok(loc, delimiter);
    // printf("%s\n", strtok(NULL, delimiter));
    char* not_final = strtok(NULL, delimiter);
    printf("%s\n", strstr(serve_file_path, "."));
    if (strstr(not_final, ".") == NULL) {
        strcat(serve_file_path, not_final);
        strcat(serve_file_path, "index.html");
    } else strcat(serve_file_path, not_final);

    printf("%s\n", serve_file_path);

    bzero(buff, MAX); 
    n = 0;

    FILE* serve;
    if ((serve = fopen(serve_file_path, "rb")) == 0) {
        char* h;
        craft_header(&h, "404 Not Found", 0);
        write(sockfd, h, strlen(h));

        close(sockfd);
        exit(-1);
    }

    unsigned int file_size;
    fseek(serve, 0, SEEK_END);
    file_size = ftell(serve);
    fseek(serve, 0, SEEK_SET);

    char* header;
    craft_header(&header, "200 OK", file_size);
    // printf("%s\n", header);
    write(sockfd, header, strlen(header));

    // copy server message in the buffer 
    while (fread(buff, 1, sizeof(buff), serve)) {
        // and send that buffer to client
        write(sockfd, buff, strlen(buff));
    }

    printf("Terminating!\n");
    close(sockfd);
    fclose(serve);
    exit(0);
} 

// Driver function 
int main(int argc, char** argv) 
{   
    int port = PORT;
    if (argc == 2) port = atoi(argv[1]);

	int sockfd, connfd, len; 
	struct sockaddr_in servaddr, cli; 

	// socket create and verification 
	sockfd = socket(AF_INET, SOCK_STREAM, 0); 
	if (sockfd == -1) { 
		printf("socket creation failed...\n"); 
		exit(0); 
	} 
	else
		printf("Socket successfully created..\n"); 
	bzero(&servaddr, sizeof(servaddr)); 

    if (setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, &(int){1}, sizeof(int)) < 0)
        fprintf(stderr ,"setsockopt(SO_REUSEADDR) failed");

	// assign IP, PORT 
	servaddr.sin_family = AF_INET; 
	servaddr.sin_addr.s_addr = htonl(INADDR_ANY); 
	servaddr.sin_port = htons(port); 

	// Binding newly created socket to given IP and verification 
	if ((bind(sockfd, (SA*)&servaddr, sizeof(servaddr))) != 0) { 
		printf("socket bind failed...\n"); 
		exit(0); 
	} 
	else
		printf("Socket successfully binded..\n"); 

	// Now server is ready to listen and verification 
	if ((listen(sockfd, 5)) != 0) { 
		printf("Listen failed...\n"); 
		exit(0); 
	} 
	else
		printf("Server listening...\n"); 
	len = sizeof(cli); 

	// Accept the data packet from client and verification 
    while (1) {
        connfd = accept(sockfd, (SA*)&cli, &len);
        if (connfd < 0) { 
            printf("server acccept failed...\n"); 
            exit(0); 
        } 
        else
            printf("server acccept the client...\n"); 
        
        if (fork() == 0)
            func(connfd);
        else connfd = 0;
    }

	// After chatting close the socket 
	close(sockfd); 
} 
