%{

#include "y.tab.h"
#include <stdio.h>
#include <readline/readline.h>
#include <readline/history.h>

static int mygetinput(char *buf,size_t result, size_t max_size) {
	size_t n;
	char *line = readline("myshell> ");
	for ( n = 0; n < strlen(line) && n < max_size - 1; ++n)
		buf[n] = line[n];
	buf[n++] = '\n';
	add_history(line);
	free(line);
	result = n;
	return result;
}  

#define YY_INPUT(buf,result,max_size) result = mygetinput(buf, result, max_size);


%}
%option noyywrap
%option yylineno

%%

\n 		{
			return NEWLINE;
		}

[ \t]* 	{     	}	

">" 	{
			return GREAT;
		}

">&"	{
			return GREATAMPERSAND;
		}

">>"	{
			return GREATGREAT;
		}

">>&"	{
			return GREATGREATAMPERSAND;
		}

"<"	 	{
			return LESS;
		}

"<<"	{
			return LESSLESS;
		}

"2>" 	{
  			return TWOGREAT;
		}

"|"		{
			return PIPE;
		}

"&"		{
			return AMPERSAND;
		}

[\%\=\+\'\"\\\(\)\$\/\_\-\.\?\*\~a-zA-Z0-9]+	{
													yylval.string_val = yytext;
													return WORD;
												}

%%