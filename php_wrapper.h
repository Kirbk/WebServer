

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>
#include <errno.h>
#include <string.h>

#define die(e) do { fprintf(stderr, "%s\n", e); exit(EXIT_FAILURE); } while (0);

char * run_script(char * file_name, char * q_string) {
    char query[strlen(q_string + 16)];
    sprintf(query, "QUERY_STRING=%s", q_string);
    char *argv[] = { "/usr/bin/php-cgi", "-f", file_name, 0 };
    // char *envp[] =
    // {
    //     "HTTP_USER_AGENT=fuckshit",
    //     "PATH=/bin:/usr/bin",
    //     "TZ=UTC0",
    //     "USER=beelzebub",
    //     "LOGNAME=tarzan",
    //     q_string,
    //     0
    // };

    char ** envp = calloc(2, sizeof(char*));
    envp[0] = malloc(strlen(query + 1) * sizeof(char));
    envp[1] = malloc(1);

    strcpy(envp[0], query);
    memset(envp[1], 0, 1);

    printf("%s\n", envp[0]);

    // printf("%d\n", execve(argv[0], &argv[0], envp));
    // perror("");

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
        // printf("%s\n", envp[0]);
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
            //printf("%.*s", nbytes, foo);

            if (ret_val == NULL) {
                ret_val = (char*)malloc(nbytes * sizeof(char) + 1);
                memset(ret_val, 0, nbytes + 1);
                strncpy(ret_val, foo, nbytes);

                ret_size = nbytes + 1;
            } else {
                ret_val = (char*) realloc(ret_val, (ret_size += nbytes));
                memset(ret_val + ret_size - 1, 0, 1);
                // printf("%s\n", ret_val);
                
                strncat(ret_val, foo, nbytes);
                // printf("%s\n", ret_val);
            }
        }
        memset(ret_val + ret_size - 1, 0, 1);
        //printf("%s\n", ret_val);
        wait(NULL);
    
    }

    free(envp[0]);
    free(envp);

    return ret_val;
}
