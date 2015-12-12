%option noyywrap

%{
#include "tree.h"
#include "parser.tab.h"
#include <stdlib.h>
%}


DIGIT [0-9]
ID [a-zA-Z][A-Za-z0-9]*


%%
[\t\n]+

{DIGIT}+ {yylval.numero= atoi(yytext); return(NUM);}

"int main()"	{return(MAIN);}

";"		{return(SEMICOLON);}

"="		 {return(ASSIGN);}
"=="	 {return(EQ);}
">"		 {return(G);}
"<"		 {return(L);}
"<="	 {return(LEQ);}
">="	 {return(GEQ);}
"!=" 	 {return(NOTEQ);}

"||"	 {return(OR);}
"&&"	 {return(AND);}

"true"	 {return(TRUE);}
"false"	 {return(FALSE);}

"+"		 {return(Plus);}
"-"		 {return(Minus);}
"*" 	 {return(Times);}
"/" 	 {return(Div);}

"if" 	 {return(IF);}
"else" 	 {return(ELSE);}
"while"	 {return(WHILE);}


"int" 	 {return(INT);}
"bool"	 {return(BOOL);}

"("		 {return(P0);}
")"		 {return(P1);}

"{"		{return(CB0);}
"}"		{return(CB1);}

"return" {return(RETURN);}

{ID}+ 	 {yylval.ident = strdup(yytext); return(VAR);}
