#ifndef __SHELL_H__
#define __SHELL_H__

#include "command.h"

typedef struct Shell {
    Command *currentCommand;
    CommandList *currentCommandList;
    char *currentDir;
} Shell;


#endif