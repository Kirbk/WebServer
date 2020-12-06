#include "php_wrapper.h"

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>
#include <errno.h>
#include <string.h>
#include <ctype.h>

#include "config.h"
#include "util.h"
#include "status_code_macros.h"
#include "status_codes.h"

#define ENV_FIELDS (23)
#define MAX_ROOT (2048)

char * create_env_from_header(key_value_pair kp) {
    char* lwr = kp.key;
    for ( ; *lwr; ++lwr) *lwr = toupper(*lwr);
    lwr = NULL;

    char * ret = calloc(strlen(kp.key) + strlen(kp.value) + 2, sizeof(char));
    strcpy(ret, kp.key);
    strcat(ret, "=");
    strcat(ret, kp.value);

    return ret;
}

char ** create_environment(char * file_name, char * q_string, http_request_header * header) {
    char * doc_root;
    char * root_directory;
    option_setting_pair* home_directory;
    if ((home_directory = get_option("HomeDir")) != NULL) {
        char wor_dir[MAX_ROOT] = { 0 };

        if (home_directory->settings[0][0] == '.') getcwd(wor_dir, MAX_ROOT);
        
        if (strcmp(wor_dir, "") == 0) {
            doc_root = calloc(strlen(home_directory->settings[0]) + 32, sizeof(char));
            root_directory = calloc(strlen(home_directory->settings[0]) + 1, sizeof(char));
            strcpy(doc_root, "DOCUMENT_ROOT=");
            strcat(doc_root, home_directory->settings[0]);
            strcpy(root_directory, home_directory->settings[0]);
        }
        else {
            doc_root = calloc(strlen(home_directory->settings[0]) + strlen(wor_dir) + 32, sizeof(char));
            root_directory = calloc(strlen(home_directory->settings[0]) + strlen(wor_dir) + 1, sizeof(char));
            strcpy(doc_root, "DOCUMENT_ROOT=");
            strcat(doc_root, wor_dir);
            strcpy(root_directory, wor_dir);
        }
    } else {
        char cur_dir[MAX_ROOT];
        doc_root = calloc(MAX_ROOT + 32, sizeof(char));
        getcwd(cur_dir, MAX_ROOT);
        
        strcpy(doc_root, "DOCUMENT_ROOT=");
        strcat(doc_root, cur_dir);
    }

    char ** envp = calloc(ENV_FIELDS + 1, sizeof(char *));

    if (q_string) {
        char query[strlen(q_string) + 16];
        sprintf(query, "QUERY_STRING=%s", q_string);

        envp[0] = calloc(strlen(query) + 1, sizeof(char));
        strcpy(envp[0], query);
    } else {
        envp[0] = calloc(strlen("QUERY_STRING=") + 1, sizeof(char));
        strcpy(envp[0], "QUERY_STRING=");
    }

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
    
    int server_protocol_length = (header->version) ? strlen(header->version) + 32 : 32;
    char server_protocol[server_protocol_length];
    sprintf(server_protocol, "SERVER_PROTOCOL=%s", header->version);

    int request_method_length = (header->method) ? strlen(method_type_strings[header->method]) + 32 : 32;
    char request_method[request_method_length];
    sprintf(request_method, "REQUEST_METHOD=%s", method_type_strings[header->method]);

    int script_uri_length = strlen(header->host) + strlen(header->resource) + 32;
    char script_uri[script_uri_length];
    sprintf(script_uri, "SCRIPT_URI=%s%.*s", header->host, (int)strcspn(header->resource, "?"), header->resource);

    char * s_fn = file_name;

    if (s_fn[0] == '.') {
        s_fn += 1;
        if (s_fn[0] == '/') s_fn += 1;
    }

    int request_uri_length = strlen(header->resource) + 32;
    char request_uri[request_uri_length];
    sprintf(request_uri, "REQUEST_URI=%s", header->resource);

    int php_self_length = strlen(s_fn) + 32;
    char php_self[php_self_length];
    sprintf(php_self, "PHP_SELF=/%s", s_fn);

    int script_name_length = strlen(s_fn) + 32;
    char script_name[script_name_length];
    sprintf(script_name, "SCRIPT_NAME=/%s", s_fn);

    int script_filename_length = strlen(doc_root) + strlen(file_name) + 32;
    char script_filename[script_filename_length];
    sprintf(script_filename, "SCRIPT_FILENAME=%s/%s", root_directory, s_fn);

    char content_length[128];
    sprintf(content_length, "CONTENT_LENGTH=%d", (header->method == POST) ? header->content_length : 0);

    int content_type_length = (header->content_type) ? strlen(header->content_type) + 32 : 32;
    char content_type[content_type_length];
    sprintf(content_type, "CONTENT_TYPE=%s", (header->content_type) ? header->content_type : "");

    int cookie_length = (header->cookie) ? strlen(header->cookie) + 32 : 32;
    char cookie[cookie_length];
    sprintf(cookie, "HTTP_COOKIE=%s", (header->cookie) ? header->cookie : "");

    int env_length = 21;


    int i = 0;
    if (header->additional) {
        while (header->additional[i].key != NULL && header->additional[i].value != NULL) {
            env_length++;
            char * env_var = create_env_from_header(header->additional[i]);
            envp = realloc(envp, (env_length + 1) * sizeof(char *));

            envp[env_length - 1] = env_var;

            i++;
        }
    }


    envp[1] = calloc(strlen(script_url) + 1, sizeof(char));
    envp[2] = calloc(strlen(http_host) + 1, sizeof(char));
    envp[3] = calloc(strlen(http_user_agent) + 1, sizeof(char));
    envp[4] = calloc(strlen(http_accept) + 1, sizeof(char));
    envp[5] = calloc(strlen(http_accept_language) + 1, sizeof(char));
    envp[6] = calloc(strlen(http_accept_encoding) + 1, sizeof(char));
    envp[7] = calloc(strlen(http_dnt) + 1, sizeof(char));
    envp[8] = calloc(strlen(http_connection) + 1, sizeof(char));
    envp[9] = calloc(strlen(http_upgrade_insecure) + 1, sizeof(char));
    envp[10] = calloc(strlen(server_protocol) + 1, sizeof(char));
    envp[11] = calloc(strlen(request_method) + 1, sizeof(char));
    envp[12] = calloc(strlen(request_uri) + 1, sizeof(char));
    envp[13] = calloc(strlen(php_self) + 1, sizeof(char));
    envp[14] = calloc(strlen(script_name) + 1, sizeof(char));
    envp[15] = calloc(strlen(script_filename) + 1, sizeof(char));
    envp[16] = doc_root;
    envp[17] = calloc(strlen(content_length) + 1, sizeof(char));
    envp[18] = calloc(strlen(content_type) + 1, sizeof(char));
    envp[19] = calloc(strlen(script_uri) + 1, sizeof(char));
    envp[20] = calloc(strlen(cookie) + 1, sizeof(char));

    envp[env_length] = 0;

    strcpy(envp[1], script_url);
    strcpy(envp[2], http_host);
    strcpy(envp[3], http_user_agent);
    strcpy(envp[4], http_accept);
    strcpy(envp[5], http_accept_language);
    strcpy(envp[6], http_accept_encoding);
    strcpy(envp[7], http_dnt);
    strcpy(envp[8], http_connection);
    strcpy(envp[9], http_upgrade_insecure);
    strcpy(envp[10], server_protocol);
    strcpy(envp[11], request_method);
    strcpy(envp[12], request_uri);
    strcpy(envp[13], php_self);
    strcpy(envp[14], script_name);
    strcpy(envp[15], script_filename);
    strcpy(envp[17], content_length);
    strcpy(envp[18], content_type);
    strcpy(envp[19], script_uri);
    strcpy(envp[20], cookie);


    free(root_directory);

    return envp;
}

char * run_script(char * file_name, char * q_string, char * post_data, http_request_header * header, http_response_header * response) {
    if (file_name == NULL || header == NULL) return NULL;

    char *argv[] = { "/usr/bin/php-cgi", "-q", "-d", "cgi.force_redirect=0", 0 };
    char ** envp = create_environment(file_name, q_string, header);

    // int i = 0;
    // while (envp[i]) printf("%s\n", envp[i++]);

    int link[2];
    int err_link[2];
    int write_link[2];
    pid_t pid;
    char foo[4096];
    char * ret_val = NULL;
    int ret_size = 0;

    if (pipe(link)==-1)
        die("pipe");
    if (pipe(err_link)==-1)
        die("pipe");
    if (pipe(write_link)==-1)
        die("pipe");

    if ((pid = fork()) == -1)
        die("fork");

    if(pid == 0) {
        close(link[0]);
        close(write_link[1]);
        close(err_link[0]);
        dup2(write_link[0], STDIN_FILENO);
        dup2(link[1], STDOUT_FILENO);
        dup2(err_link[1], STDERR_FILENO);
        
        close(link[1]);
        close(err_link[1]);
        close(write_link[0]);

        execve(argv[0], &argv[0], envp);
        perror("");

        die("execve");

    } else {
        if (post_data) {
            write(write_link[1], post_data, strlen(post_data));
        }
        close(link[1]);
        close(err_link[1]);
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

        memset(foo, 0, sizeof(foo));
        nbytes = 0;
        while ((nbytes = read(err_link[0], foo, sizeof(foo))) > 0) {
            ret_val = (char*) realloc(ret_val, (ret_size += nbytes));
            memset(ret_val + ret_size - 1, 0, 1);
            strncat(ret_val, foo, nbytes);
        }

        memset(ret_val + ret_size - 1, 0, 1);
        wait(NULL);
    }

    for (int i = 0; i < ENV_FIELDS; ++i) if (envp[i]) free(envp[i]);
    if (envp) free(envp);


    int cont = (ret_val) ? 1 : 0;
    int start = 0;
    int cur = 0;
    while (cont) {
        cur = start;
        while (ret_val[cur] && ret_val[cur] != '\r') {
            if (ret_val[cur + 1] && ret_val[cur + 1] == '\n') break;
            cur++;
        }

        if (ret_val[cur + 2] && ret_val[cur + 2] == '\r')
            if (ret_val[cur + 3] && ret_val[cur + 3] == '\n')
                cont = 0;

        int token_loc = get_occurrence_n(ret_val + start, ':', 1);
        if (token_loc > 0) {
            char key[token_loc + 1];
            char option[(cur - 1) - token_loc - start];

            memset(key, 0, sizeof(key));
            memset(option, 0, sizeof(option));

            for (int i = 0; i < sizeof(key) - 1; i++) key[i] = ret_val[start + i];
            for (int i = 0; i < sizeof(option) - 1; i++) option[i] = ret_val[start + token_loc + i + 2];

            if (strcmp(key, "Status") == 0) {
                response->status = calloc(strlen(option) + 1, sizeof(char));
                strcpy(response->status, option);
            }
        }
        
        start = cur + 2;
    }


    // char * new_line = calloc(strlen(ret_val) + 1, sizeof(char));
    // char * orig = new_line;
    // strcpy(new_line, ret_val + cur + 4); // Jump past trailing CRLFs
    
    // ret_val = realloc(ret_val, strlen(new_line) + 1);
    // memset(ret_val, 0, strlen(new_line) + 1);
    // strncpy(ret_val, new_line, strlen(new_line));

    response->content_length = strlen(ret_val) - cur - 5;


    // free(orig);

    return ret_val;
}