%defines
%error-verbose
%{
	#include <iostream>
	using namespace std;
	bool result;
	int yylex (void);
	void * load_string(const char *);
	void yyerror (const char* str);
	void clean_up(void * buffer_state);
%}

%code requires { #include "parser.hpp" }
%union 
{
	int int_val;
	bool bool_val;
}

%token L_PAREN R_PAREN NEG AND OR IMP EQU VAR ERR
%token<int_val> CONSTANT
%type<bool_val> line 
%type<bool_val>exp 
%left EQU
%right IMP
%left OR 
%left AND 

%precedence NEG
%start line

%%

line:
	   exp				    {result = $1;}
	  ;

exp:
	  CONSTANT 				{$$ = ($1 != 0);  }
	| exp EQU exp			{$$ = ($1 == $3); }
	| exp IMP exp			{$$ = (!$1 || $3);}
	| exp OR exp			{$$ = ($1 || $3); }
	| exp AND exp			{$$ = ($1 && $3); }
	| NEG exp %prec NEG		{$$ = !$2;        }
	| L_PAREN exp R_PAREN	{$$ = $2;         }
	;

%%

void yyerror (const char* str)
{

}


int parseExpression(const char *expression)
{
	void * buffer_state = load_string(expression);

	int status = yyparse();

	if(buffer_state)
		clean_up(buffer_state);
	
	if(!status)
		return result;
	else
		return -1;
}
