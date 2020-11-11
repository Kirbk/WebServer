#include "http_header.h"

#include <unistd.h>

#include "log.h"

http_response_header create_http_response_header() {
    http_response_header head = { CLOSE, 
                                { INVALID, INVALID, INVALID, INVALID, INVALID, INVALID, INVALID, INVALID, INVALID },
                                "", // Automatically set DATE field
                                "",
                                "",
                                "",
                                "",
                                "",
                                { 5, 1000 },
                                -1, 
                                -1 };

    return head;
}

void add_line_c(int count, char* base, ...) {
    va_list args;
    va_start(args, base);

    for (int i = 0; i < count; ++i) {
        strcat(base, va_arg(args, char*));
    }

    va_end(args);

    strcat(base, "\r\n");
}

void add_line(char* base, char* added) {
    add_line_c(1, base, added);
}

int count_lines(char const *str)
{
    char const *p = str;
    int count;
    for (count = 0; ; ++count) {
        p = strstr(p, "\r\n");
        if (!p)
            break;
        p = p + 2;
    }
    return count - 1;
}


http_request_header* parse_request_header(char* header_text) {
    http_request_header* header = malloc(sizeof(http_request_header));
    memset(header, 0, sizeof(http_request_header));

    info(header_text);

    char line_end[strlen(header_text + 1)];
    strcpy(line_end, header_text);


    char* lines[count_lines(line_end)];

    const char s[3] = "\r\n";
    char* token = strtok(line_end, s);


    for (int i = 0; i < k; i++) {
        lines[i] = calloc(strlen(token) + 1, sizeof(char));
        strcpy(lines[i], token);
        token = strtok(NULL, s);
    }

    const char space[2] = " ";
    char* top = strtok(lines[0], space);
    for (int i = 0; i < METHOD_TYPES; i++)
        if (!strcmp(top, method_type_strings[i])) header->method = i;

    char* important = strtok(NULL, space);
    header->resource = calloc(strlen(important) + 1, sizeof(char));
    strcpy(header->resource, important);

    important = strtok(NULL, space);
    header->version = calloc(strlen(important) + 1, sizeof(char));
    strcpy(header->version, important);

    for (int i = 1; i < count_lines(header_text); i++) {
        const char sep[3] = ": ";
        char* key = strtok(lines[i], sep);
        char* value = strtok(NULL, sep);

        if (!strcmp(key, "A-IM")) {
            header->aim = calloc(strlen(value) + 1, sizeof(char));
            strcpy(header->aim, value);
        } else if (!strcmp(key, "Accept")) {
            header->accept = calloc(strlen(value) + 1, sizeof(char));
            strcpy(header->accept, value);
        } else if (!strcmp(key, "Accept-Charset")) {
            header->accept_charset = calloc(strlen(value) + 1, sizeof(char));
            strcpy(header->accept_charset, value);
        } else if (!strcmp(key, "Accept-Datetime")) {
            header->accept_date_time = calloc(strlen(value) + 1, sizeof(char));
            strcpy(header->accept_date_time, value);
        } else if (!strcmp(key, "Accept-Encoding")) {
            header->accept_encoding = calloc(strlen(value) + 1, sizeof(char));
            strcpy(header->accept_encoding, value);
        } else if (!strcmp(key, "Accept-Language")) {
            header->accept_language = calloc(strlen(value) + 1, sizeof(char));
            strcpy(header->accept_language, value);
        } else if (!strcmp(key, "Accept-Control-Request-Method")) {
            header->access_control_request_method = calloc(strlen(value) + 1, sizeof(char));
            strcpy(header->access_control_request_method, value);
        } else if (!strcmp(key, "Authorization")) {
            header->authorization = calloc(strlen(value) + 1, sizeof(char));
            strcpy(header->authorization, value);
        } else if (!strcmp(key, "Cache-Control")) {
            header->cache_control = calloc(strlen(value) + 1, sizeof(char));
            strcpy(header->cache_control, value);
        } else if (!strcmp(key, "Connection")) {
            for (int i = 0; i < CONNECTION_TYPES; i++)
                if (!strcmp(value, connection_type_strings[i])) header->connection = i;
        } else if (!strcmp(key, "Content-Encoding")) {
            header->content_encoding = calloc(strlen(value) + 1, sizeof(char));
            strcpy(header->content_encoding, value);
        } else if (!strcmp(key, "Content-Length")) {
            header->content_length = atoi(value);
        } else if (!strcmp(key, "Content-MD5")) {
            header->content_md5 = calloc(strlen(value) + 1, sizeof(char));
            strcpy(header->content_md5, value);
        } else if (!strcmp(key, "Content-Type")) {
            header->content_type = calloc(strlen(value) + 1, sizeof(char));
            strcpy(header->content_type, value);
        } else if (!strcmp(key, "Cookie")) {
            header->cookie = calloc(strlen(value) + 1, sizeof(char));
            strcpy(header->cookie, value);
        } else if (!strcmp(key, "Date")) {
            header->date = calloc(strlen(value) + 1, sizeof(char));
            strcpy(header->date, value);
        } else if (!strcmp(key, "Expect")) {
            header->expect = calloc(strlen(value) + 1, sizeof(char));
            strcpy(header->expect, value);
        } else if (!strcmp(key, "Forwarded")) {
            header->forwarded = calloc(strlen(value) + 1, sizeof(char));
            strcpy(header->forwarded, value);
        } else if (!strcmp(key, "From")) {
            header->from = calloc(strlen(value) + 1, sizeof(char));
            strcpy(header->from, value);
        } else if (!strcmp(key, "Host")) {
            header->host = calloc(strlen(value) + 1, sizeof(char));
            strcpy(header->host, value);
        } else if (!strcmp(key, "HTTP2-Settings")) {
            header->http2_settings = calloc(strlen(value) + 1, sizeof(char));
            strcpy(header->http2_settings, value);
        } else if (!strcmp(key, "If-Match")) {
            header->if_match = calloc(strlen(value) + 1, sizeof(char));
            strcpy(header->if_match, value);
        } else if (!strcmp(key, "If-Modified-Since")) {
            header->if_modified_since = calloc(strlen(value) + 1, sizeof(char));
            strcpy(header->if_modified_since, value);
        } else if (!strcmp(key, "If-None-Match")) {
            header->if_none_match = calloc(strlen(value) + 1, sizeof(char));
            strcpy(header->if_none_match, value);
        } else if (!strcmp(key, "If-Range")) {
            header->if_range = calloc(strlen(value) + 1, sizeof(char));
            strcpy(header->if_range, value);
        } else if (!strcmp(key, "If-Unmodified-Since")) {
            header->if_unmodified_since = calloc(strlen(value) + 1, sizeof(char));
            strcpy(header->if_unmodified_since, value);
        } else if (!strcmp(key, "Max-Forwards")) {
            header->max_forwards = calloc(strlen(value) + 1, sizeof(char));
            strcpy(header->max_forwards, value);
        } else if (!strcmp(key, "Origin")) {
            header->origin = calloc(strlen(value) + 1, sizeof(char));
            strcpy(header->origin, value);
        } else if (!strcmp(key, "Pragma")) {
            header->pragma = calloc(strlen(value) + 1, sizeof(char));
            strcpy(header->pragma, value);
        } else if (!strcmp(key, "Proxy-Authorization")) {
            header->proxy_authorization = calloc(strlen(value) + 1, sizeof(char));
            strcpy(header->proxy_authorization, value);
        } else if (!strcmp(key, "Range")) {
            header->range = calloc(strlen(value) + 1, sizeof(char));
            strcpy(header->range, value);
        } else if (!strcmp(key, "Referer")) {
            header->referer = calloc(strlen(value) + 1, sizeof(char));
            strcpy(header->referer, value);
        } else if (!strcmp(key, "TE")) {
            header->te = calloc(strlen(value) + 1, sizeof(char));
            strcpy(header->te, value);
        } else if (!strcmp(key, "Trailer")) {
            header->trailer = calloc(strlen(value) + 1, sizeof(char));
            strcpy(header->trailer, value);
        } else if (!strcmp(key, "Transfer-Encoding")) {
            header->transfer_encoding = calloc(strlen(value) + 1, sizeof(char));
            strcpy(header->transfer_encoding, value);
        } else if (!strcmp(key, "User-Agent")) {
            header->user_agent = calloc(strlen(value) + 1, sizeof(char));
            strcpy(header->user_agent, value);
        } else if (!strcmp(key, "Upgrade")) {
            header->upgrade = calloc(strlen(value) + 1, sizeof(char));
            strcpy(header->upgrade, value);
        } else if (!strcmp(key, "Via")) {
            header->via = calloc(strlen(value) + 1, sizeof(char));
            strcpy(header->via, value);
        } else if (!strcmp(key, "Warning")) {
            header->warning = calloc(strlen(value) + 1, sizeof(char));
            strcpy(header->warning, value);
        }
    }

    return header;
}

int construct_response_header(char** header_text, http_response_header* h) {
    if (h == NULL) {
        fprintf(stderr, "Header struct is NULL!\n");
        return -1;
    }
    if (header_text == NULL) {
        fprintf(stderr, "Header text pointer is NULL!\n");
        return -2;
    }
    if (h->status == NULL || !strcmp(h->status, "")) {
        fprintf(stderr, "Status is required!\n");
        return -3;
    }

    char craft[MAX_HEADER_LENGTH] = "HTTP/1.1 ";
    add_line(craft, h->status);

    add_line_c(2, craft, "Connection: ", connection_type_strings[h->connection]);

    if (h->connection == KEEP_ALIVE) {
        char keep_alive_buf[32];
        sprintf(keep_alive_buf, "Keep-Alive: time=%d, max=%d", h->keep_alive.timeout, h->keep_alive.max_requests);
        add_line(craft, keep_alive_buf);
    }

    if (strcmp(h->date, ""))
        add_line_c(2, craft, "Date: ", h->date);
    
    if (strcmp(h->content_type, "")) 
        add_line_c(2, craft, "Content-Type: ", h->content_type);
    
    if (strcmp(h->location, ""))
        add_line_c(2, craft, "Location: ", h->location);
    
    if (strcmp(h->cookie, ""))
        add_line_c(2, craft, "Set-Cookie: ", h->cookie);
    
    if (strcmp(h->download_file, ""))
        add_line_c(3, craft, "Content-Disposition: attachment; filename=\"", h->download_file, "\"");

    if (h->content_length > 0) {
        char len[10];
        sprintf(len, "%d", h->content_length);
        add_line_c(2, craft, "Content-Length: ", len); 
    }

    if (h->age > 0) {
        char age[10];
        sprintf(age, "%d", h->age);
        add_line_c(2, craft, "Age: ", age); 
    }

    add_line(craft, ""); // Add trailing CRLF

    *header_text = calloc(strlen(craft) + 1, sizeof(char));
    memset(*header_text, 0, sizeof(char)); // Clear to be sure
    strcpy(*header_text, craft);

    return 0;
}

char* construct_response_header_c(http_response_header* h) {
    char* header;
    construct_response_header(&header, h);
    return header;
}

int launch_and_discard(int sockfd, char** header) {
    int c = write(sockfd, *header, strlen(*header));
    free(*header);

    return c;
}

void free_request_header(http_request_header** header) {
    if (!(*header)) return;
    http_request_header* head = *header;
    char* p;
    if ((p = head->content_encoding)) free(p);
    if ((p = head->content_md5)) free(p);
    if ((p = head->content_type)) free(p);
    if ((p = head->cookie)) free(p);
    if ((p = head->date)) free(p);
    if ((p = head->expect)) free(p);
    if ((p = head->forwarded)) free(p);
    if ((p = head->from)) free(p);
    if ((p = head->host)) free(p);
    if ((p = head->http2_settings)) free(p);
    if ((p = head->if_match)) free(p);
    if ((p = head->if_modified_since)) free(p);
    if ((p = head->if_none_match)) free(p);
    if ((p = head->if_range)) free(p);
    if ((p = head->if_unmodified_since)) free(p);
    if ((p = head->max_forwards)) free(p);
    if ((p = head->origin)) free(p);
    if ((p = head->pragma)) free(p);
    if ((p = head->proxy_authorization)) free(p);
    if ((p = head->range)) free(p);
    if ((p = head->referer)) free(p);
    if ((p = head->resource)) free(p);
    if ((p = head->te)) free(p);
    if ((p = head->trailer)) free(p);
    if ((p = head->transfer_encoding)) free(p);
    if ((p = head->upgrade)) free(p);
    if ((p = head->user_agent)) free(p);
    if ((p = head->version)) free(p);
    if ((p = head->via)) free(p);
    if ((p = head->warning)) free(p);

    free(*header);
    *header = NULL;
}