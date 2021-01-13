%{
#include "compiler.hpp"
#include "symbols/symbols.hpp"
#include "values/values.hpp"
#include "expressions/expressions.hpp"
#include "conditions/conditions.hpp"

extern int yylex();
extern int yylineno;
extern FILE* yyin;
int yyerror(const string str);
void error(string str);

%}

%define parse.error verbose

%union {
    std::string *pidentifier;
    long num;
    Value* value;
    Expression* expression;
    Condition* condition;
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
%type <condition> condition
%type <instructions> command
%type <instructions> commands

//Operators precedence
%left PLUS MINUS
%left TIMES DIV MOD      



%%
program:

      DECLARE declarations _BEGIN commands END             { writeCommands($4); }
    | _BEGIN commands END                                  { writeCommands($2); }
    ;

declarations:

      declarations','pidentifier                           { getSymbolTable()->addValueVariable(*$3); }
    | declarations','pidentifier'('num':'num')'            { getSymbolTable()->addArrayVariable(*$3, $5, $7); }
    | pidentifier                                          { getSymbolTable()->addValueVariable(*$1); }
    | pidentifier'('num':'num')'                           { getSymbolTable()->addArrayVariable(*$1, $3, $5); }
    ;

commands:

      commands command                                     { $$ = mergeInstructions($1, $2); }
    | command                                              { $$ = $1; }
    ;

command:

      identifier ASSIGN expression';'                      { $$ = assign($1, $3); }
    | IF condition THEN commands ELSE commands ENDIF       { $$ = ifThenElse($2, $4, $6); }
    | IF condition THEN commands ENDIF                     { $$ = ifThen($2, $4); }
    | WHILE condition DO commands ENDWHILE                 { $$ = whileDo($2, $4); }
    | REPEAT commands UNTIL condition';'                   { $$ = repeatUntil($2, $4); }
    | FOR pidentifier FROM value TO value                  { scopeInvoke(); }
      DO commands ENDFOR                                   { scopeRevoke(); }
    | FOR pidentifier FROM value DOWNTO value              { scopeInvoke(); }
      DO commands ENDFOR                                   { scopeRevoke(); }
    | READ identifier';'                                   { $$ = read($2); }
    | WRITE value';'                                       { $$ = write($2); }
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

      value EQ value                                       { $$ = getEqCondition($1, $3); }
    | value NEQ value                                      { $$ = getNeqCondition($1, $3); }
    | value LE value                                       { $$ = getLeCondition($1, $3); }
    | value GE value                                       { $$ = getGeCondition($1, $3); }
    | value LEQ value                                      { $$ = getLeqCondition($1, $3); }
    | value GEQ value                                      { $$ = getGeqCondition($1, $3); }
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
    if( argv != 3 ) {
        cerr << "Wywołanie: kompilator <plik_źródłowy> <plik_wynikowy>" << endl;
        return 1;
    }

    yyin = fopen(argc[1], "r");
    if (yyin == NULL) {
        cerr<<"Nie znaleziono podanego pliku"<<endl;
        return 1;
    }

    setOutputFilename(argc[2]);
    
    yyparse();
      
    cout << "Skompilowano do '" << argc[2] <<"'"<< endl;

    return 0;
}

int yyerror(string err) {
    int line = yychar == YYEMPTY ? yylineno - 1 : yylineno;
    cout<<yychar<<endl;
    cout << "Błąd w lini " << line << ": " << err << endl;
    exit(EXIT_FAILURE);
}

void error(string err) {
    int line = yychar == 259 ? yylineno - 1 : yylineno;
    cerr << "Błąd w lini " << line << ": " << err << endl;
    exit(EXIT_FAILURE);
}
