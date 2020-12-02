#include <stdio.h> 
#include <netdb.h> 
#include <netinet/in.h> 
#include <arpa/inet.h>
#include <stdlib.h> 
#include <string.h> 
#include <unistd.h>
#include <sys/socket.h> 
#include <sys/types.h>
#include <sys/stat.h>
#include <time.h>

#include "log.h"
#include "config.h"
#include "client.h"

int create_config_file(char* config_file_path) {
    FILE* f;
    if ((f = fopen(config_file_path, "w")) == NULL)
        return -1;

    char* lines[] = { "HomeDir ./", "Port 80", "RandomPort no", "DefaultIndex index.html", "MaxQueueLength 10" };

    for (int i = 0; i < (sizeof(lines) / sizeof(char*)); i++) {
        fwrite(lines[i], 1, strlen(lines[i]), f);
        fwrite("\n", 1, 1, f);
    }

    fclose(f);

    return 0;
}

int find_configuration(char** config_file_name) {
    struct stat st = {0};
    char config_file_path[strlen(*config_file_name) + 25]; // Extra barrier for default path
    if (access(*config_file_name, F_OK) == -1) {
        warning("Config file not found!");
        strcpy(config_file_path, *config_file_name);
        info("Checking to see if name has valid path...");
        char* c = strrchr(config_file_path, '/');
        if (c == NULL) {
            info("No path found, using default(./config/)...");
            strcpy(config_file_path, "./config/");
        } else {
            config_file_path[c - config_file_path] = '\0';
            info("Attempting to create directory found in passed name...");
            if ((stat(config_file_path, &st) == -1)) {
                if ((mkdir(config_file_path, 0700) == -1)) {
                    warning("Unable to create directory");
                    info("No path found, using default(./config/)...");
                    strcpy(config_file_path, "./config/");
                } else info("Succesfully created config directory...");
            } else {
                info("Succesfully found config directory...");
                info("Creating config file...");
                if (create_config_file(*config_file_name) == -1) {
                    warning("Unable to create config file!");
                    info("Trying to use default(./config/)...");
                    strcpy(config_file_path, "./config/");
                }
            }
        }

        info(config_file_path);
        struct stat final = {0};
        if (stat(config_file_path, &final) == -1) {
            if (mkdir(config_file_path, 0700) == -1) {
                fatal("Unable to create folder! Bigger problem here.");
                return -1;
            }
        }
        strcat(config_file_path, "/serv.conf");
    } else strcpy(config_file_path, *config_file_name);

    if (access(config_file_path, F_OK) == -1) {
        // Create config file
        if (create_config_file(config_file_path) == -1) {
            fatal("Unable to create config file!");
            return -1;
        }

        info("Successfully created config file...");
    }

    return 0;
}

int configure(char* config_file_name, 
              char** home_dir, 
              int* port,
              int* max_queue_length) {
    
    if (find_configuration(&config_file_name) == -1) return -1;
    if (parse_config_file(config_file_name) == -1) return -1;

    option_setting_pair* hd;
    if ((hd = get_option("HomeDir")) != NULL) {
        info("Setting home directory, this might cause errors after this log...");
        *home_dir = hd->settings[0];
        additional(*home_dir);
    }

    option_setting_pair* pt;
    if ((pt = get_option("Port")) != NULL) {
        info("Setting port number, remember to keep it in range, some ports are privelaged...");
        *port = atoi(pt->settings[0]);
        additional(pt->settings[0]);
        additional("If there are characters in this, they are ignored by atoi()");
    }

    option_setting_pair* rand_port;
    if ((rand_port = get_option("RandomPort")) != NULL) { // Intentionally overwrite port option
        if (!strcmp(rand_port->settings[0], "yes")) {
            info("Setting port to random value...");
            srand(time(0));
            *port = (rand() % 65535) + 1;
            
            char buf[6];
            sprintf(buf, "%d", *port);
            additional(buf);
        } // Ignore incorrect input
    }

    option_setting_pair* ql;
    if ((ql = get_option("MaxQueueLength")) != NULL) {
        info("Adjusting max queue length");
        *max_queue_length = atoi(ql->settings[0]);
        
        char buf[10];
        sprintf(buf, "%d", *max_queue_length);
        additional(buf);
    }

    return 0;
}

void usage() {
    printf("Usage: calserv [flag] [option]\n");
    printf("\t-p, --port <port number>\n");
    printf("\t-w, --working-dir <working directory>\n");
    printf("\t-c, --config <config file name>\n");
    exit(-1);
}

int main(int argc, char** argv) {
    char* config_file_name = "./config/serv.conf";
    char* home_dir = ".";
    int port = 80;
    int verbose = 0; // Boolean flag
    int max_queue_length = 10;

    if (configure(config_file_name, &home_dir, &port, &max_queue_length) < 0) {
        logger("Unable to configure web server! See previous errors.", 2);
        exit(-1);
    }

    // Grab args
    for (int i = 1; i < argc; i++) {
        if (!(strcmp(argv[i], "-p")) || !(strcmp(argv[i], "--port"))) {
            if (argv[i+1] == NULL || argv[i+1][0] == '-') usage();
            info("Port changed due to CLI arg");
            additional(argv[i+1]);
            port = atoi(argv[++i]);
        } else if (!(strcmp(argv[i], "-w")) || !(strcmp(argv[i], "--working-dir"))) {
            if (argv[i+1] == NULL || argv[i+1][0] == '-') usage();
            home_dir = argv[++i];
        } else if (!(strcmp(argv[i], "-c")) || !(strcmp(argv[i], "--config"))) {
            if (argv[i+1] == NULL || argv[i+1][0] == '-') usage();
            config_file_name = argv[++i];
        } else if (!(strcmp(argv[i], "-v")) || !(strcmp(argv[i], "--verbose"))) {
            verbose = 1;
        }
    }
    if (!verbose) freopen("/dev/null", "w", stderr);

    int sockfd, connfd;
    socklen_t len;
    struct sockaddr_in servaddr, cli;

    if ((sockfd = socket(AF_INET, SOCK_STREAM, 0)) == -1) {
        fatal("Unable to create socket!");
        system_out();
        exit(-1);
    }

    info("Successfully created socket...");
    memset(&servaddr, 0, sizeof(servaddr));

    if (setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, &(int){1}, sizeof(int)) < 0) {
        warning("Weird... Unable to reuse address...");
        system_out();
        info("Continuing... Could cause bind errors if server restarted on same port\n");
    }

    // assign IP, PORT
	servaddr.sin_family = AF_INET;
	servaddr.sin_addr.s_addr = htonl(INADDR_ANY);
	servaddr.sin_port = htons(port);

	// Binding newly created socket to given IP and verification 
	if ((bind(sockfd, (struct sockaddr*)&servaddr, sizeof(servaddr))) != 0) { 
		fatal("Failed to bind to socket!");
        system_out();
		exit(0);
	} 
	else
		info("Bound to socket..."); 

    // Now server is ready to listen and verification 
	if ((listen(sockfd, max_queue_length)) != 0) { 
		fatal("Failed to listen!"); 
		exit(0); 
	} 
	else
		info("Server listening..."); 
	len = sizeof(cli); 

    while (1) {
        connfd = accept(sockfd, (struct sockaddr*)&cli, &len);
        if (connfd < 0) { 
            if (verbose) {
                info("Client could not be accepted...");
                system_out();
            }

            connfd = 0;
        } 
        else {
            struct sockaddr_in addr;
            socklen_t addr_size = sizeof(struct sockaddr_in);
            int res = getpeername(connfd, (struct sockaddr *)&addr, &addr_size);
            char clientip[20];
            strcpy(clientip, inet_ntoa(addr.sin_addr));

            info_v("Connection Established!");
            additional_v(clientip);
            if (fork() == 0) {
                dispatch(connfd, clientip);
            } else {
                close(connfd);
                connfd = 0;
            }
        }
    }

    close(sockfd);
}