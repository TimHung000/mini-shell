%{
	#include <stdio.h>
	#include <stdlib.h> // exit, malloc
	#include <string.h> // strlen
	#include "shell.h"
	#include "command.h"
	extern void yyerror(const char * s);
	extern int yylex();

	extern Shell *myshell;
%}

%token <string_val> WORD
%token 	NEWLINE GREAT GREATAMPERSAND GREATGREAT GREATGREATAMPERSAND LESS LESSLESS TWOGREAT PIPE  AMPERSAND
%union	{
			char *string_val;
		}

%%
goal:
	line_command goal 
	{
	}
	| line_command
	{
	}

line_command:
	command_list io_modifier_list background NEWLINE  
	{ 
		commandListExecute(myshell->currentCommandList);
    	commandListDestroy(myshell->currentCommandList);
		myshell->currentCommandList = commandListCreate();
	}
	| command_list background NEWLINE 
	{ 
		commandListExecute(myshell->currentCommandList);
    	commandListDestroy(myshell->currentCommandList);
		myshell->currentCommandList = commandListCreate();
	}
	| NEWLINE 
	{ 
	}
	;

command_list:
	command_list PIPE command_args
		{ 
			commandListAppend(myshell->currentCommandList, myshell->currentCommand);
			myshell->currentCommand = commandCreate();
		}
	| command_args 	
		{ 
			commandListAppend(myshell->currentCommandList, myshell->currentCommand);
			myshell->currentCommand = commandCreate();
		}
	;

command_args:
	command arg_list 
	|
	;

arg_list:
	arg_list argument 
	|
	;

argument:
	WORD 
	{
		char *temp = (char*)malloc(sizeof(char) * (strlen($1)+1));
		memcpy(temp, $1, strlen($1)+1);
		commandAppend(myshell->currentCommand, temp);
	}
	;

command:
	WORD 
	{
		char *temp = (char*)malloc(sizeof(char) * (strlen($1)+1));
		memcpy(temp, $1, strlen($1)+1);
		commandAppend(myshell->currentCommand, temp);
	}
	;

io_modifier_list:
	io_modifier_list iomodifier_opt
	| iomodifier_opt
	;

iomodifier_opt:
	GREAT WORD {
		char *temp = (char*)malloc(sizeof(char) * (strlen($2)+1));
		memcpy(temp, $2, strlen($2)+1);
		commandListRedirect(myshell->currentCommandList, 1, temp);
	}

	| GREATGREAT WORD {
		char *temp = (char*)malloc(sizeof(char) * (strlen($2)+1));
		memcpy(temp, $2, strlen($2)+1);
		commandListRedirect(myshell->currentCommandList, 1, temp);
		myshell->currentCommandList->isAppend = true;
	}
	| GREATAMPERSAND WORD {
		char *temp1 = (char*)malloc(sizeof(char) * (strlen($2)+1));
		memcpy(temp1, $2, strlen($2)+1);
		char *temp2 = (char*)malloc(sizeof(char) * (strlen($2)+1));
		memcpy(temp2, $2, strlen($2)+1);
		commandListRedirect(myshell->currentCommandList, 1, temp1);
		commandListRedirect(myshell->currentCommandList, 2, temp2);
	}
	| GREATGREATAMPERSAND WORD {
		char *temp1 = (char*)malloc(sizeof(char) * (strlen($2)+1));
		memcpy(temp1, $2, strlen($2)+1);
		char *temp2 = (char*)malloc(sizeof(char) * (strlen($2)+1));
		memcpy(temp2, $2, strlen($2)+1);
		commandListRedirect(myshell->currentCommandList, 1, temp1);
		commandListRedirect(myshell->currentCommandList, 2, temp2);
		myshell->currentCommandList->isAppend = true;
	}
	| LESS WORD {
		char *temp = (char*)malloc(sizeof(char) * (strlen($2)+1));
		memcpy(temp, $2, strlen($2)+1);
		commandListRedirect(myshell->currentCommandList, 0, temp);
	}

	| LESSLESS WORD {
		char *temp = (char*)malloc(sizeof(char) * (strlen($2)+1));
		memcpy(temp, $2, strlen($2)+1);
		commandListRedirect(myshell->currentCommandList, 0, temp);
		myshell->currentCommandList->isAppend = true;
	}
	| TWOGREAT WORD {
		char *temp = (char*)malloc(sizeof(char) * (strlen($2)+1));
		memcpy(temp, $2, strlen($2)+1);
		commandListRedirect(myshell->currentCommandList, 2, temp);
	}
	;

background:
	AMPERSAND {
		myshell->currentCommandList->isBackground = true; 
	}
	|
	;

%%

void yyerror(const char *s) {
    fprintf(stderr, "%s\n", s);
	fflush(stdout);
}
