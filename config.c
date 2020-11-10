#include <stdlib.h>

#include "config.h"
#include "log.h"

option_setting_pair* create_pair(char* option, char** settings, int settings_c) {
    option_setting_pair* pair = malloc(sizeof(option_setting_pair));

    pair->option = calloc(strlen(option), sizeof(char));
    strcpy(pair->option, option);

    pair->settings = calloc(settings_c, sizeof(char*));
    for (int i = 0; i < settings_c; i++) {
        pair->settings[i] = malloc(strlen(settings[i]) + 1);
        strcpy(pair->settings[i], settings[i]);
    }
    pair->settings_length = settings_c;

    return pair;
}

int process_option(char* option_setting) {
    const char delimeter[2] = " ";
    char* option = strtok(option_setting, delimeter);
    if (option == NULL) {
        warning("Option not set to anything... Somehow?");
        return -1;
    }
    char* setting = strtok(NULL, delimeter);
    if (setting == NULL) {
        warning("Setting not set to anything!");
        additional(setting);
        memset(option_setting, '\0', sizeof(char) * MAX_LINE_LENGTH);
        return -1;
    }
    
    if (strcmp(option, "HomeDir") == 0) // Incorrect directory handled by serving code
        add_option(create_pair(option, &setting, 1));
    else if (strcmp(option, "RandomPort") == 0) // Invalid port handled by socket code
        add_option(create_pair(option, &setting, 1));
    else if (strcmp(option, "Port") == 0)
        add_option(create_pair(option, &setting, 1));
    else {
        fprintf(stderr, "%s[WARNING] Option (%s) not valid! Ignoring...%s\n", KYEL, option, DEF);
        memset(option_setting, '\0', sizeof(char) * MAX_LINE_LENGTH);
        return -1;
    }

    // Reset line buffer
    memset(option_setting, '\0', sizeof(char) * MAX_LINE_LENGTH);
    return 0;
}

void add_option(option_setting_pair* osp) {
    option_setting_linked_list_node* node = malloc(sizeof(option_setting_linked_list_node));
    node->data = osp;
    node->next = NULL;

    if (option_setting_list == NULL) option_setting_list = node;
    else {
        option_setting_linked_list_node* cur_node = option_setting_list;
        while (cur_node->next != NULL) cur_node = cur_node->next;
        cur_node->next = node;
    }
}

option_setting_pair* get_option(char* option) {
    option_setting_linked_list_node* cur_node = option_setting_list;
    while (cur_node != NULL && strcmp(cur_node->data->option, option))
        cur_node = cur_node->next;
    
    return (cur_node == NULL) ? NULL : cur_node->data;
}

int parse_config_file(char* config_file_name) {
    FILE* f;
    if ((f = fopen(config_file_name, "r")) == NULL) {
        perror("Parsing Config");
        return -1;
    }

    char line[MAX_LINE_LENGTH] = { 0 };
    char c;
    int count = 0;
    int comment = 0; // Boolean flag
    while ((c = fgetc(f)) != EOF) {
        if (c == '#') comment = 1;
        if (c != '\n') {
            if (!comment)
                line[count++] = c;
        }
        else {
            if (process_option(line) == -1) {
                warning("Unable to process option! See previous errors.");
            }
            count = 0;
            comment = 0;
        }
    }

    // Support processing last line with both EOF of \n
    if (strlen(line) > 0) process_option(line);
    return 0;
}