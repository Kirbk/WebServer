#include "php_wrapper.h"

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>
#include <errno.h>
#include <string.h>

#define ENV_FIELDS (11)

char ** create_environment(char * file_name, char * q_string, http_request_header * header) {
    char query[strlen(q_string) + 16];
    sprintf(query, "QUERY_STRING=%s", q_string);

    char script_url[strlen(file_name) + 16];
    sprintf(script_url, "SCRIPT_URL=%s", file_name + 1);

    int host_length = (header->host) ? strlen(header->host) + 16 : 16;
    char http_host[host_length];
    sprintf(http_host, "HTTP_HOST=%s", header->host);

    int user_agent_length = (header->user_agent) ? strlen(header->user_agent) + 32 : 32;
    char http_user_agent[user_agent_length];
    sprintf(http_user_agent, "HTTP_USER_AGENT=%s", header->user_agent);

    int accept_length = (header->accept) ? strlen(header->accept) + 16 : 16;
    char http_accept[accept_length];
    sprintf(http_accept, "HTTP_ACCEPT=%s", header->accept);

    int accept_language_length = (header->accept_language) ? strlen(header->accept_language) + 32 : 32;
    char http_accept_language[accept_language_length];
    sprintf(http_accept_language, "HTTP_ACCEPT_LANGUAGE=%s", header->accept_language);

    int accept_encoding_length = (header->accept_encoding) ? strlen(header->accept_encoding) + 32 : 32;
    char http_accept_encoding[accept_encoding_length];
    sprintf(http_accept_encoding, "HTTP_ACCEPT_ENCODING=%s", header->accept_encoding);

    int dnt_length = (header->dnt) ? strlen(header->dnt) + 16 : 16;
    char http_dnt[dnt_length];
    sprintf(http_dnt, "HTTP_DNT=%s", header->dnt);

    int connection_length = (header->connection) ? strlen(connection_type_strings[header->connection]) + 32 : 32;
    char http_connection[connection_length];
    sprintf(http_connection, "HTTP_CONNECTION=%s", connection_type_strings[header->connection]);

    int upgrade_insecure_length = (header->upgrade_insecure) ? strlen(header->upgrade_insecure) + 32 : 32;
    char http_upgrade_insecure[upgrade_insecure_length];
    sprintf(http_upgrade_insecure, "HTTP_UPGRADE_INSECURE_REQUESTS=%s", header->upgrade_insecure);


    char ** envp = calloc(ENV_FIELDS + 1, sizeof(char *));
    envp[0] = calloc(strlen(query) + 1, sizeof(char));
    envp[1] = calloc(strlen(script_url) + 1, sizeof(char));
    envp[2] = calloc(strlen(http_host) + 1, sizeof(char));
    envp[3] = calloc(strlen(http_user_agent) + 1, sizeof(char));
    envp[4] = calloc(strlen(http_accept) + 1, sizeof(char));
    envp[5] = calloc(strlen(http_accept_language) + 1, sizeof(char));
    envp[6] = calloc(strlen(http_accept_encoding) + 1, sizeof(char));
    envp[7] = calloc(strlen(http_dnt) + 1, sizeof(char));
    envp[8] = calloc(strlen(http_connection) + 1, sizeof(char));
    envp[9] = calloc(strlen(http_upgrade_insecure) + 1, sizeof(char));
    envp[ENV_FIELDS - 1] = calloc(1, sizeof(char));
    

    strcpy(envp[0], query);
    strcpy(envp[1], script_url);
    strcpy(envp[2], http_host);
    strcpy(envp[3], http_user_agent);
    strcpy(envp[4], http_accept);
    strcpy(envp[5], http_accept_language);
    strcpy(envp[6], http_accept_encoding);
    strcpy(envp[7], http_dnt);
    strcpy(envp[8], http_connection);
    strcpy(envp[9], http_upgrade_insecure);
    memset(envp[ENV_FIELDS - 1], 0, 1);

    return envp;
}

char * run_script(char * file_name, char * q_string, http_request_header * header) {
    if (file_name == NULL || q_string == NULL || header == NULL) return NULL;

    char *argv[] = { "/usr/bin/php-cgi", "-f", file_name, 0 };
    char ** envp = create_environment(file_name, q_string, header);

    int link[2];
    pid_t pid;
    char foo[4096];
    char * ret_val = NULL;
    int ret_size = 0;

    if (pipe(link)==-1)
        die("pipe");

    if ((pid = fork()) == -1)
        die("fork");

    if(pid == 0) {
        dup2 (link[1], STDOUT_FILENO);
        close(link[0]);
        close(link[1]);
        execve(argv[0], &argv[0], envp);
        perror("");
        die("execl");

    } else {

        close(link[1]);
        int nbytes = 0;
        while ((nbytes = read(link[0], foo, sizeof(foo))) > 0) {
            if (ret_val == NULL) {
                ret_val = (char*)malloc(nbytes * sizeof(char) + 1);
                memset(ret_val, 0, nbytes + 1);
                strncpy(ret_val, foo, nbytes);

                ret_size = nbytes + 1;
            } else {
                ret_val = (char*) realloc(ret_val, (ret_size += nbytes));
                memset(ret_val + ret_size - 1, 0, 1);
                strncat(ret_val, foo, nbytes);
            }
        }
        memset(ret_val + ret_size - 1, 0, 1);
        wait(NULL);
    
    }

    for (int i = 0; i < ENV_FIELDS; ++i) if (envp[i]) free(envp[i]);
    if (envp) free(envp);

    return ret_val;
}