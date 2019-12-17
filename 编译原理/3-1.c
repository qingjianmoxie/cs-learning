/* 3-1.c */
#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h> //可变参数函数
#include "3-1.h"

ast *newast(int nodetype, ast *l, ast *r)
{
    ast *a = malloc(sizeof(ast));
    if (!a)
    {
        yyerror("out of space");
        exit(0);
    }
    a->nodetype = nodetype;
    a->l = l;
    a->r = r;
    return a;
}

ast *newnum(double d)
{
    numval *a = malloc(sizeof(numval));
    if (!a)
    {
        yyerror("out of space");
        exit(0);
    }
    a->nodetype = 'K';
    a->number = d;
    return (ast *)a;
}

double eval(ast *a)
{
    double v; /* 子树的计算结果 */
    switch (a->nodetype)
    {
    case 'K':
        v = ((numval *)a)->number;
        break;
    case '+':
        v = eval(a->l) + eval(a->r);
        break;
    case '-':
        v = eval(a->l) - eval(a->r);
        break;
    case '*':
        v = eval(a->l) * eval(a->r);
        break;
    case '/':
        v = eval(a->l) / eval(a->r);
        break;
    case '|':
        v = eval(a->l);
        if (v < 0)
            v = -v;
        break;
    case 'M':
        v = -eval(a->l);
        break;
    default:
        printf("internal error: bad node %c\n", a->nodetype);
    }
    return v;
}

void treefree(ast *a)
{
    switch (a->nodetype)
    {
    /* 两颗子树 */
    case '+':
    case '-':
    case '*':
    case '/':
        treefree(a->r);
    /* 一颗子树 */
    case '|':
    case 'M':
        treefree(a->l);
    /* 一颗子树 */
    case 'K':
        free(a);
        break;
    default:
        printf("internal error: bad node %c\n", a->nodetype);
    }
}

void yyerror(const char *s, ...)
{
    va_list ap;
    va_start(ap, s);
    fprintf(stderr, "%d: error: ", yylineno);
    vfprintf(stderr, s, ap);
    fprintf(stderr, "\n");
}

int main(void)
{
    printf("> ");
    return yyparse();
}