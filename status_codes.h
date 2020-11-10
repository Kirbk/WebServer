#ifndef __STATUS_CODES_H__
#define __STATUS_CODES_H__

#include <stdio.h>

typedef struct {
  int code;
  const char* msg;
} http_status;

static const http_status http_status_codes[] = {
       {100, "Continue"},
       {101, "Switching Protocols"},
       {200, "OK"},
       {201, "Created"},
       {202, "Accepted"},
       {203, "Non-Authoritative Information"},
       {204, "No Content"},
       {205, "Reset Content"},
       {206, "Partial Content"},
       {300, "Multiple Choices"},
       {301, "Moved Permanently"},
       {302, "Moved Temporarily"},
       {303, "See Other"},
       {304, "Not Modified"},
       {305, "Use Proxy"},
       {400, "Bad Request"},
       {401, "Unauthorized"},
       {402, "Payment Required"},
       {403, "Forbidden"},
       {404, "Not Found"},
       {405, "Method Not Allowed"},
       {406, "Not Acceptable"},
       {407, "Proxy Authentication Required"},
       {408, "Request Time-out"},
       {409, "Conflict"},
       {410, "Gone"},
       {411, "Length Required"},
       {412, "Precondition Failed"},
       {413, "Request Entity Too Large"},
       {414, "Request-URI Too Large"},
       {415, "Unsupported Media Type"},
       {428, "Precondition Required"},
       {429, "Too Many Requests"},
       {431, "Request Header Fields Too Large"},
       {451, "Unavailable For Legal Reasons"},
       {500, "Internal Server Error"},
       {501, "Not Implemented"},
       {502, "Bad Gateway"},
       {503, "Service Unavailable"},
       {504, "Gateway Time-out"},
       {505, "HTTP Version not supported"},
       {511, "Network Authentication Required"},
       {0,   (char*)0}
};

static http_status get_status_code(int status_number) {
  int c = 0;
  while (http_status_codes[c].code != 0 && http_status_codes[c].code != status_number) c++;

  return http_status_codes[c];
}


#endif