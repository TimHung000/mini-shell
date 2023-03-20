#include <stdlib.h> // getenv
#include <sys/types.h> //getpwuid getuid
#include <stdio.h> // perror malloc
#include <pwd.h> // getpwuid 
#include <unistd.h> // getuid getcwd
#include <stdlib.h> // malloc
#include "command.h"
#include "shell.h"
#include "y.tab.h"

Shell *myshell;

/*
**  Initialize shell
**  @param  argc    argument count
**  @param  argv    argument variables
**  @param  env     environment
**  return 0 if successfule, otherwise, 1;
*/
int main(int argc, char *argv[])
{   
    // get home path
    const char *homePath = NULL;
    if((homePath = getenv("HOME")) == NULL)
        homePath = (const char*)getpwuid(getuid())->pw_dir;

    // get cuurent path
    char *currentPath = getcwd(NULL, 0);
    if(homePath == NULL || currentPath == NULL)
        perror("can't get the path");

    // myshell initialize
    myshell = (Shell*)malloc(sizeof(Shell));
	myshell->currentCommand = commandCreate();
	myshell->currentCommandList = commandListCreate();
    myshell->currentDir = currentPath;

    setbuf(stdout, NULL);
    yyparse();
    return 0;
}