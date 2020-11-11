#ifndef __CONFIG_H__
#define __CONFIG_H__

#define MAX_LINE_LENGTH (100)

#include <stdio.h>
#include <string.h>

typedef struct {
    char* option;
    char** settings;
    unsigned int settings_length;
} option_setting_pair;

typedef struct oslln {
    option_setting_pair* data;
    struct oslln* next;
} option_setting_linked_list_node;

typedef option_setting_linked_list_node* osll;

static osll option_setting_list = NULL;

static char* affirm[] = {
    "yes",
    "no"
};

static char* anything[] = { "*" };
static char* numeric[] = { "#" };

static const option_setting_pair setting_choices[] = {
    { "HomeDir", anything, 1 },
    { "RandomPort", affirm, 1 },
    { "Port", numeric, 1 },
    { "DefaultIndex", anything, 1 }
};

int process_option(char* option_setting);
void add_option(option_setting_pair* osp);
option_setting_pair* get_option(char* option);
int parse_config_file(char* config_file_name);

#endif