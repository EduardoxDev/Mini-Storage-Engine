#include "parser.h"
#include <string.h>
#include <ctype.h>
#include <stdio.h>

static void trim(char* str) {
    char* start = str;
    while (isspace(*start)) start++;
    
    if (start != str) {
        memmove(str, start, strlen(start) + 1);
    }
    
    char* end = str + strlen(str) - 1;
    while (end > str && isspace(*end)) {
        *end = '\0';
        end--;
    }
}

Command parse_command(const char* input) {
    Command cmd;
    cmd.type = CMD_UNKNOWN;
    cmd.key[0] = '\0';
    cmd.value[0] = '\0';
    
    char buffer[2048];
    strncpy(buffer, input, sizeof(buffer) - 1);
    buffer[sizeof(buffer) - 1] = '\0';
    trim(buffer);
    
    char* token = strtok(buffer, " \t\n");
    if (!token) return cmd;
    
    for (char* p = token; *p; p++) {
        *p = toupper(*p);
    }
    
    if (strcmp(token, "INSERT") == 0) {
        cmd.type = CMD_INSERT;
        token = strtok(NULL, " \t\n");
        if (token) {
            strncpy(cmd.key, token, sizeof(cmd.key) - 1);
            token = strtok(NULL, "\n");
            if (token) {
                trim(token);
                strncpy(cmd.value, token, sizeof(cmd.value) - 1);
            }
        }
    } else if (strcmp(token, "SELECT") == 0) {
        cmd.type = CMD_SELECT;
        token = strtok(NULL, " \t\n");
        if (token) {
            strncpy(cmd.key, token, sizeof(cmd.key) - 1);
        }
    } else if (strcmp(token, "UPDATE") == 0) {
        cmd.type = CMD_UPDATE;
        token = strtok(NULL, " \t\n");
        if (token) {
            strncpy(cmd.key, token, sizeof(cmd.key) - 1);
            token = strtok(NULL, "\n");
            if (token) {
                trim(token);
                strncpy(cmd.value, token, sizeof(cmd.value) - 1);
            }
        }
    } else if (strcmp(token, "DELETE") == 0) {
        cmd.type = CMD_DELETE;
        token = strtok(NULL, " \t\n");
        if (token) {
            strncpy(cmd.key, token, sizeof(cmd.key) - 1);
        }
    } else if (strcmp(token, "SCAN") == 0) {
        cmd.type = CMD_SCAN;
    } else if (strcmp(token, "EXIT") == 0 || strcmp(token, "QUIT") == 0) {
        cmd.type = CMD_EXIT;
    }
    
    return cmd;
}
