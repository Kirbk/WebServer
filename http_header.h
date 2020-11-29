#ifndef __HTTP_HEADER__
#define __HTTP_HEADER__

#include <string.h>
#include <stdio.h>
#include <stdarg.h>
#include <stdlib.h>

#define MAX_HEADER_LENGTH (2000)
#define CONNECTION_TYPES (2)
#define METHOD_TYPES (9)

typedef enum {CLOSE = 0, KEEP_ALIVE = 1} connection_type;
typedef enum {INVALID = -1, GET = 0, HEAD = 1, POST = 2, PUT = 3, DELETE = 4, CONNECT = 5, OPTIONS = 6, TRACE = 7, PATCH = 8} method_type;

static char* connection_type_strings[] = { "close", "keep-alive" };
static char* method_type_strings[] = { "GET", "HEAD", "POST", "PUT", "DELETE", "CONNECT", "OPTIONS", "TRACE", "PATCH" };

typedef struct {
    connection_type connection;
    method_type allow[9]; // GET, HEAD (Method Not Found Error)
    char* date; // Must be included for http standard.
    char* status; // 200 OK
    char* content_type; // text/html; charset=utf-8
    char* location; // Redirection
    char* cookie;
    char* download_file;

    struct {
        int timeout;
        int max_requests;
    } keep_alive;

    int content_length;
    int age;
} http_response_header;

typedef struct {
    connection_type connection;
    method_type method;

    unsigned int content_length;

    char* resource;
    char* version;

    char* aim; // Acceptable instance-manipulations for the request.
    char* accept; // text/html
    char* accept_charset; // utf-8
    char* accept_date_time;
    char* accept_encoding;
    char* accept_language; // en-US
    char* access_control_request_method;
    char* access_control_request_headers;
    char* authorization;
    char* cache_control;
    char* content_encoding;
    char* content_md5;
    char* content_type;
    char* cookie;
    char* date;
    char* expect;
    char* forwarded;
    char* from;
    char* host;
    char* http2_settings;
    char* if_match;
    char* if_modified_since;
    char* if_none_match;
    char* if_range;
    char* if_unmodified_since;
    char* max_forwards;
    char* origin;
    char* pragma;
    char* proxy_authorization;
    char* range;
    char* referer;
    char* te;
    char* trailer;
    char* transfer_encoding;
    char* user_agent;
    char* upgrade;
    char* via;
    char* warning;
} http_request_header;

http_response_header create_http_response_header();
void add_line_c(int count, char* base, ...);
void add_line(char* base, char* added);
http_request_header* parse_request_header(char* hdr_txt);
void free_request_header(http_request_header** header);
int construct_response_header(char** header_text, http_response_header* h, int is_php);
char* construct_response_header_c(http_response_header* h, int is_php);
int launch_and_discard(int sockfd, char** header);

#endif