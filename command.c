#define _POSIX_C_SOURCE  200809L // for dprintf
#define _GNU_SOURCE  // for dprintf
#include "command.h"
#include "shell.h"
#include <stdlib.h> // malloc EXIT_FAILURE
#include <string.h> // strcat strlen
#include <stdio.h>
#include <unistd.h> // pipe fork chdir
#include <sys/types.h> // pid_t open
#include <sys/wait.h> // waitpid
#include <sys/stat.h> // open
#include <fcntl.h> //openq O_RONLY O_WONLY O_RDWR O_CREAT
#include <errno.h>

extern Shell *myshell;

void fd_is_valid(int fd)
{
    if(!(fcntl(fd, F_GETFD) != -1 || errno != EBADF))
        perror("fd not good");
    // return fcntl(fd, F_GETFD) != -1 || errno != EBADF;
}

Command* commandCreate()
{
    Command* self = (Command*)calloc(1,sizeof(Command));
    if(!self)
        perror("can't allocate memory in commandCreate");
    self->commandArgs = NULL;
    self->commandLength = 0;
    self->commandCurrentLength = 0;
    return self;
}

void commandDestroy(Command *self)
{
    for(int i = 0; i < self->commandCurrentLength; ++i)
    {
        free(self->commandArgs[i]);
    }
    free(self->commandArgs);
    free(self);
}

char **getCommandArgs(Command *self)
{
    return self->commandArgs;
}

void commandAppend(Command *self, char *input)
{
    if(self -> commandLength == self -> commandCurrentLength)
    {
        self -> commandLength = self -> commandLength + 5;
        self -> commandArgs = (char**)realloc(self->commandArgs,sizeof(char*) * self -> commandLength);
    }
    self->commandArgs[self->commandCurrentLength] = input;
    ++self->commandCurrentLength;
    for(int i = self->commandCurrentLength; i < self->commandLength; ++i)
        self->commandArgs[i] = NULL;
}

bool isInArray(char** array , int size, char* input) {
    for(int i = 0; i < size; ++i)
    {
        if(strcmp(array[i], input) == 0)
            return true;
    }
    return false;
}


CommandList* commandListCreate()
{
    CommandList *self = (CommandList*)calloc(1, sizeof(CommandList));
    self -> commands = NULL;
    self -> commandsLength = 0;
    self -> commandsCurrentLength = 0;
    bool isBackground = false;
    bool isAppend = false;
    char *inFile = NULL;
    char *outFile = NULL;
    char *errFile = NULL;
}

void commandListDestroy(CommandList *self)
{
    free(self->inFile);
    free(self->outFile);
    free(self->errFile);
    for(int i = 0; i < self->commandsCurrentLength; ++i)
    {
        commandDestroy(self->commands[i]);
    }
    free(self->commands);
    free(self);
}


void commandListClear(CommandList *self)
{
    free(self->inFile);
    self->inFile = NULL;
    free(self->outFile);
    self->outFile = NULL;
    free(self->errFile);
    self->errFile = NULL;

    bool isBackground = false;
    bool isAppend = false;
    for(int i = 0; i < self->commandsCurrentLength; ++i)
    {
        commandDestroy(self->commands[i]);
    }
    self->commandsCurrentLength = 0;
}

void commandListAppend(CommandList* self, Command *command)
{
    if(self -> commandsLength == self -> commandsCurrentLength)
    {
        self -> commandsLength = self -> commandsLength + 5;
        self -> commands = (Command**)realloc(self->commands,sizeof(Command*) * self -> commandsLength);
    }
    self->commands[self->commandsCurrentLength] = command;
    ++self->commandsCurrentLength;
    for(int i = self->commandsCurrentLength; i < self->commandsLength; ++i)
        self->commands[i] = NULL;
}

void check(int fd)
{
    if(!isatty(fd))
        perror("not terminal");
}
void commandListExecute(CommandList* self)
{

    if(strcmp(getCommandArgs(self->commands[0])[0], "exit") == 0)
        exit(EXIT_SUCCESS);
    else if(strcmp(getCommandArgs(self->commands[0])[0], "cd") == 0)
    {
        chdir(getCommandArgs(self->commands[0])[1]);
        myshell->currentDir = getcwd(NULL, 0);
        commandListClear(self);
        return;
    }

    int fd[2];
    int defaultIn = dup(STDIN_FILENO);
	int defaultOut = dup(STDOUT_FILENO);
	int defaultErr = dup(STDERR_FILENO);
    int fdIn;
    int fdOut;
    int fdErr;
    int status;




    // setting the input stream
    if ( self->inFile ) {
        fdIn = open(self->inFile, O_RDONLY);
        if (fdIn < 0) {
            fprintf(stderr, "cannot open %s: No such file\n", self->inFile);
            commandListClear(self);
            return;
        }

    }
    else {
        fdIn = dup(defaultIn);
    }


    
    for(int i = 0; i < self->commandsCurrentLength; ++i)
    {
        // setting input from last command
        dup2(fdIn, STDIN_FILENO);

        close(fdIn);

        // setting stream
        if (i < self->commandsCurrentLength - 1)
        {
            if(pipe(fd) < 0)
            {
                fprintf(stderr, "can't create pipe");
                fflush(stderr);
                exit(1);
            }
            fdIn = fd[0];
            fdOut = fd[1];
            dup2(fdOut, STDOUT_FILENO);
            close(fdOut);
        }
        else /* the command*/
        {
            // output stream
            if(self->outFile)
            {
                printf("outfile: %s\n", self->outFile);
                if(self->isAppend)
                    fdOut = open(self->outFile, O_CREAT|O_WRONLY|O_APPEND, 0664);
                else
                    fdOut = open(self->outFile, O_CREAT|O_WRONLY|O_TRUNC, 0664);
            }
            else
                fdOut = dup(defaultOut);


            // error stream
            if(self->errFile)
            {
                if(self->isAppend)
                    fdErr = open(self->errFile, O_CREAT|O_WRONLY|O_APPEND, 0644);
                else
                    fdErr = open(self->errFile, O_CREAT|O_WRONLY|O_TRUNC, 0644);
            }
            else
                fdErr = dup(defaultErr);

            dup2(fdOut, STDOUT_FILENO);
            close(fdOut);
            dup2(fdErr, STDERR_FILENO);
            close(fdErr);


        }


        pid_t pid = fork();
        if (pid < 0)
        {
            perror("fail rot fork");
        }
        else if (pid == 0)
        {
            execvp(getCommandArgs(self->commands[i])[0], getCommandArgs(self->commands[i]));
            exit(EXIT_FAILURE);
        }
        else
        {
            if(!self->isBackground)
            {
                waitpid(pid, NULL, 0);

            }
            else
            {
                dprintf(defaultOut, "child Id: %d\n", pid);
            }
        }

    }
    

    // restore stdard io
    dup2(defaultIn, STDIN_FILENO);
    dup2(defaultOut, STDOUT_FILENO);
    dup2(defaultErr, STDERR_FILENO);
    close(defaultIn);
    close(defaultOut);
    close(defaultErr);
}


void commandListRedirect(CommandList *self, int i, char *file) {
    if (i==0) {
        if ( self->inFile ) {
            perror("setting multiple input redirect.\n");
            exit(EXIT_FAILURE);
        } else {
            self->inFile = file;
        }
    }
    if (i==1) {
        if ( self->outFile ) {
            perror("setting multiple error redirect.\n");
            exit(1);
        } else {
            self->outFile = file;
        }
    }
    if (i==2) {
        if ( self->errFile ) {
            perror("setting multiple output redirect.\n");
            exit(1);
        } else {
            self->errFile = file;
        }
    }
}