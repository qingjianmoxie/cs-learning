/* 3-3.y */
/* 基于抽象语法树的计算器 */
%{
#include <stdio.h>
#include <stdlib.h>
#include "3-3.h"
%}

%union {
    ast *a;
    double d;
    symbol *s; /* 指定符号 */
    symlist *sl;
    int fn;    /* 指定函数 */
}

/* declare tokens */
%token  <d> NUMBER
%token <s> NAME
%token <fn> FUNC /* 表示内置函数 */
%token  EOL
%token IF THEN ELSE WHILE DO LET

%nonassoc <fn> CMP
%right '='
%left '+' '-'
%left '*' '/'
%nonassoc '|' UMINUS

%type <a> exp stmt list explist
%type <sl> symlist

%start calclist
%%
calclist: /* 空规则 */
	| calclist stmt EOL {
        printf("= %4.4g\n", eval($2));  /* 计算抽象语法树并打印结果 */
        treefree($2);                   /* 释放抽象语法树 */
        printf("> ");
    }
    | calclist LET NAME '(' symlist ')' '=' list EOL {
        dodef($3, $5, $8);
        printf("Defined %s\n> ", $3->name);
    }
    | calclist error EOL { yyerrok; printf("> "); }
    | calclist EOL { printf("> "); }    /* 空行或者注释 */
	;

stmt: IF exp THEN list { $$ = newflow('I', $2, $4, NULL); }
    | IF exp THEN list ELSE list { $$ = newflow('I', $2, $4, $6); }
    | WHILE exp DO list { $$ = newflow('W', $2, $4, NULL); }
    | exp
    ;

list: /* 空 */ { $$ = NULL; }
    | stmt ';' list {
        if(NULL == $3)
            $$ = $1;
        else
            $$ = newast('L', $1, $3);
    }
    ;

exp: exp CMP exp { $$ = newcmp($2, $1, $3); }
    | exp '+' exp { $$ = newast('+', $1, $3); }
	| exp '-' exp { $$ = newast('-', $1, $3); }
    | exp '*' exp { $$ = newast('*', $1, $3); }
	| exp '/' exp { $$ = newast('/', $1, $3); }
    | '|' exp { $$ = newast('|', $2, NULL); }
	| '(' exp ')' { $$ = $2; }
    | '-' exp %prec UMINUS { $$ = newast('M', $2, NULL); }
    | NUMBER { $$ = newnum($1); }
    | NAME { $$ = newref($1); }
    | NAME '=' exp { $$ = newasgn($1, $3); }
    | FUNC '(' explist ')' { $$ = newfunc($1, $3); }
    | NAME '(' explist ')' { $$ = newcall($1, $3); }
    ;

explist: exp
    | exp ',' explist { $$ = newsymlist($1, $3); }
    ;

symlist: NAME { $$ = newsymlist($1, NULL); }
    | NAME ',' symlist { $$ = newsymlist($1, $3); }

%%