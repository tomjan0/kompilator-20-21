%{
#include "compiler.hpp"
#include "symbols.hpp"
#include "values/values.hpp"
#include "expressions/expressions.hpp"

#define YYDEBUG 1
long errors = 0;

extern int yylex();
extern int yylineno;
extern FILE* yyin;
int yyerror(const string str);
%}

%define parse.error verbose

%union {
    std::string *pidentifier;
    long num;
    Value* value;
    Expression* expression;
    vector<string>* instructions;
}

%token DECLARE _BEGIN END
%token IF WHILE DO REPEAT UNTIL
%token FOR 
%token THEN ELSE ENDIF FROM TO DOWNTO ENDFOR ENDWHILE ENDDO
%token READ WRITE       
%token LE GE LEQ GEQ EQ NEQ
%token ASSIGN
%token ERROR
%token <pidentifier> pidentifier
%token <num> num

// %type <variable> value
// %type <variable> identifier

%type <value> value
%type <value> identifier

%type <expression> expression
%type <cond> condition
%type <instructions> command
%type <instructions> commands

//Operators precedence
%left PLUS MINUS
%left TIMES DIV MOD      



%%
program:

      DECLARE declarations _BEGIN commands END             {}
    | _BEGIN commands END                                  {}
    ;

declarations:

      declarations','pidentifier                           { getSymbolTable()->addValueVariable(*$3); }
    | declarations','pidentifier'('num':'num')'            { getSymbolTable()->addArrayVariable(*$3, $5, $7); }
    | pidentifier                                          { getSymbolTable()->addValueVariable(*$1); }
    | pidentifier'('num':'num')'                           { getSymbolTable()->addArrayVariable(*$1, $3, $5); }
    ;

commands:

      commands command                                     {}
    | command                                              {}
    ;

command:

      identifier ASSIGN expression';'                      { assign($1, $3); }
    | IF condition THEN commands ELSE commands ENDIF       {}
    | IF condition THEN commands ENDIF                     {}
    | WHILE condition DO commands ENDWHILE                 {}
    | REPEAT commands UNTIL condition';'                   {}
    | FOR pidentifier FROM value TO value                  { scopeInvoke(); }
      DO commands ENDFOR                                   { scopeRevoke(); }
    | FOR pidentifier FROM value DOWNTO value              { scopeInvoke(); }
      DO commands ENDFOR                                   { scopeRevoke(); }
    | READ identifier';'                                   { read($2); }
    | WRITE value';'                                       { write($2); }
    ;

expression:

      value                                                { $$ = getSimpleExpression($1); }
    | value PLUS value                                     { $$ = getAddExpression($1, $3); }
    | value MINUS value                                    { $$ = getSubExpression($1, $3); }
    | value TIMES value                                    { $$ = getMulExpression($1, $3); }
    | value DIV value                                      { $$ = getDivExpression($1, $3); }
    | value MOD value                                      { $$ = getModExpression($1, $3); }
    ;

condition:

      value EQ value                                       {}
    | value NEQ value                                      {}
    | value LE value                                       {}
    | value GE value                                       {}
    | value LEQ value                                      {}
    | value GEQ value                                      {}
    ;

value:

      num                                                  { $$ = getValue($1); }
    | identifier                                           { $$ = $1; }
    ;

identifier:

      pidentifier                                          { $$ = getValue(*$1); }
    | pidentifier'('pidentifier')'                         { $$ = getValue(*$1, *$3); }
    | pidentifier'('num')'                                 { $$ = getValue(*$1, $3); }
    ;

%%


int main(int argv, char* argc[]) {
    // yydebug = 1;
    test();
    if( argv != 3 ) {
        cerr << "Aby wywołać musisz wpisać: kompilator wejśce wyjście" << endl;
        return 1;
    }

    yyin = fopen(argc[1], "r");
    if (yyin == NULL) {
        cout<<"Nie znaleziono podanego pliku"<<endl;
        return 1;
    }

    set_output_filename(argc[2]);
    open_file();
    
	yyparse();
    
    printCommands();
    writeCommands();
   if(errors == 0) {
        cout << "Udało się skompilować" << endl;
   } else {
       cout << "Kompilacja nie powiodła się. Liczba błędów: "<<errors<<endl;
   }
//    print_sym();
//    printAll();
   //cout<<"hello"<<endl;
  
	return 0;
}

// int yyerror (char const *s)
// {
//   fprintf (stderr, "%s\n", s);
// }

int yyerror(string err) {
    int line = yychar == YYEMPTY ? yylineno - 1 : yylineno;
    cout<<yychar<<endl;
    cout << "Błąd w lini " << line << ": " << err << endl;
    return 1;
    // exit(1);
}

void error(string str) {
    errors++;
    int line = yychar == 259 ? yylineno - 1 : yylineno;
    cout << "Błąd w lini " << line << ": " << str << endl;
    // exit(1);
}


void error(string str, int lineno) {
    errors++;
    cout << "YYLINENO Bład! - linia " << yylineno << ": " << str << endl;
    // exit(1);
}