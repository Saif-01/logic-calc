%option noyywrap
%option header-file="lexer.hpp"

%{
	#include <stdlib.h>
	#include "parser.hpp"
%}

%%

[ \t\n]+				;
[a-zA-Z][a-zA-Z0-9_]*	{return VAR;}
[0-1]					{yylval.int_val = atoi(yytext); return CONSTANT;}
"("						{return L_PAREN;}
")"						{return R_PAREN;}
"!"						{return NEG;}
"&"						{return AND;}
"|"						{return OR;}
"->"					{return IMP;}
"<->"					{return EQU;}
.						{return ERR;}
[2-9][0-9]*				{return ERR;}

%%

void * load_string(const char* str)
{
    YY_BUFFER_STATE buffer_state = yy_scan_string(str);
    yy_switch_to_buffer(buffer_state);
    return buffer_state;
}

void clean_up(void *buffer_state)
{
    yy_delete_buffer(static_cast<YY_BUFFER_STATE>(buffer_state));
}
