
#include <stdio.h>

#include "http_header.h"
#include "config.h"
#include "status_codes.h"

void construct_response_header_test() {
    http_response_header header = create_http_response_header();
    header.connection = KEEP_ALIVE;
    header.status = "200 OK";

    char* header_text = construct_response_header_c(&header);

    printf("%s\n", header_text);
}

void parse_config_file_test() {
    parse_config_file("serv.conf");
}

void get_status_code_test() {
    http_status h = get_status_code(404);
    printf("%d %s\n", h.code, h.msg);
}

// Must be done after config file test
void get_option_test() {
    option_setting_pair* p = get_option("RandomPort");
    if (p)
        printf("%s : %s\n", p->option, p->settings[0]);
}

int main() {
    construct_response_header_test();
    parse_config_file_test();
    get_status_code_test();
    get_option_test();
}