%option noyywrap
%option yylineno
%option nounput

%{
#include <iostream>
#include <string>
#include <vector>
#include "values/values.hpp"
#include "expressions/expressions.hpp"
#include "compiler.hpp"
#include "parser.h"

%}

IDENTIFIER              [_a-z]
NUMBER                  [-]?([1-9][0-9]*|0)
MARK                    [\(\)\:\;,]

%x COMMENT

%%
"DECLARE"               { return DECLARE; }
"BEGIN"                 { return _BEGIN; }
"END"                   { return END; }

"READ"                  { return READ; }
"WRITE"                 { return WRITE; }

"IF"                    { return IF; }
"THEN"                  { return THEN; }
"ELSE"                  { return ELSE; }
"ENDIF"                 { return ENDIF; }

"WHILE"                 { return WHILE; }
"DO"                    { return DO; }
"REPEAT"                { return REPEAT; }
"UNTIL"                 { return UNTIL; }
"ENDWHILE"              { return ENDWHILE; }

"FOR"                   { return FOR; }
"FROM"                  { return FROM; }
"TO"                    { return TO; }
"DOWNTO"                { return DOWNTO; }
"ENDFOR"                { return ENDFOR; }

":="                    { return ASSIGN; }

"="                     { return EQ; }
"!="                    { return NEQ; }
"<"                     { return LE; }
">"                     { return GE; }
"<="                    { return LEQ; }
">="                    { return GEQ; }

"+"                     { return PLUS; }
"-"                     { return MINUS; }
"/"                     { return DIV; }
"%"                     { return MOD; }
"*"                     { return TIMES; }

{IDENTIFIER}+           {   
                            yylval.pidentifier = new std::string(yytext);
                            return pidentifier;
                        }

{NUMBER}+               { 
                            yylval.num = std::stoll(yytext);
                            return num; 
                        }

{MARK}                  { return yytext[0];}

\[.*\]                  {}

"["                     { BEGIN(COMMENT); }

<COMMENT>{
    [^\]]               {}
    "]"                 { BEGIN(INITIAL); }
}

[ \t\r\n]+              {}
.                       { return ERROR; }
%%