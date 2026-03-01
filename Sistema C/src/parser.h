#ifndef PARSER_H
#define PARSER_H

typedef enum {
    CMD_INSERT,
    CMD_SELECT,
    CMD_UPDATE,
    CMD_DELETE,
    CMD_SCAN,
    CMD_EXIT,
    CMD_UNKNOWN
} CommandType;

typedef struct {
    CommandType type;
    char key[256];
    char value[1024];
} Command;

Command parse_command(const char* input);

#endif
