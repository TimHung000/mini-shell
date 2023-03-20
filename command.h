#ifndef __COMMAND_H__
#define __COMMAND_H__
#include <stdbool.h>

typedef struct Command
{
    char **commandArgs;
    int commandLength;
    int commandCurrentLength;
} Command;

Command *commandCreate();
void commandDestroy(Command *self);
void commandAppend(Command *self, char *input);
char **getCommandArgs(Command* self);
bool isInArray(char* [], int size, char* input);

typedef struct CommandList
{
    Command **commands;
    int commandsLength;
    int commandsCurrentLength;
    bool isBackground;
    bool isAppend;
    char *inFile;
    char *outFile;
    char *errFile;
} CommandList;

CommandList* commandListCreate();
void commandListDestroy(CommandList *self);
void commandListAppend(CommandList* self, Command *command);
void commandListExecute(CommandList *self);
void commandListClear(CommandList *self);
void commandListRedirect(CommandList *self, int i, char *file);

#endif