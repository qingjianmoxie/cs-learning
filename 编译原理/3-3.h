/* 3-3.h */
/* 3-3计算器的声明部分 */

/* 与词法分析器的接口 */
extern int yylineno;
void yyerror(const char *s, ...);

/* 符号表 */
typedef struct symbol
{
    char *name;
    double value;
    ast *func; /* 函数体 */
    symlist *syms; /* 虚拟参数列表 */
} symbol;

/* 固定大小的简单符号表 */
#define NHASH 9997
symbol symtab[NHASH];
symbol *lookup(char *);

/* 符号列表, 作为参数列表 */
typedef struct symlist
{
    symbol *sym;
    struct symlist *next;
} symlist;

symlist *newsymlist(symbol *sym, symlist *next);
void symlistfree(symlist *sl);

/*  节点类型
    + - * / |
    0-7 比较操作符, 位编码: 04 等于, 02 小于, 01 大于
    M 单目负号
    L 表达式或者语句列表
    I IF语句
    W WHILE语句
    N 符号引用
    = 赋值
    S 符号列表
    F 内置函数调用
    C 用户函数调用
*/
typedef enum bifs /* 内置函数 */
{
    B_sqrt = 1,
    B_exp,
    B_log,
    B_print
} bifs;

/* 抽象语法树中的节点 */
/* 所有节点都有公共的初始nodetype */
typedef struct ast
{
    int nodetype;
    struct ast *l;
    struct ast *r;
} ast;

typedef struct fncall /* 内置函数 */
{
    int nodetype;    /* 类型 F */
    ast *l;
    bifs functype;
}

typedef struct ufncall /* 用户自定义函数 */
{
    int nodetype;      /* 类型 C */
    ast *l;            /* 参数列表 */
    symbol *s;
} ufncall;

typedef struct flow
{
    int nodetype; /* 类型 I 或者 W */
    ast *cond;    /* 条件 */
    ast *tl;      /* then分支或者do语句 */
    ast *el;      /* 可选的else分支 */
} flow;

typedef struct numval
{
    int nodetype; /* 类型K 表明常量 */
    double number;
} numval;

typedef struct symref /* 符号引用 */
{
    int nodetype; /* 类型 N */
    symbol *s;
} symref;

typedef struct symasgn /* 赋值 */
{
    int notdetype; /* 类型 = */
    symbol *s;
    ast *v;        /* 值 */
} symasgn;

/* 构造抽象语法树 */
ast *newast(int nodetype, ast *l, ast *r);
ast *newcmp(int cmptype, ast *l, ast*r);
ast *newfunc(int functype, ast *l);
ast *newcall(symbol *s, ast *l);
ast *newref(symbol *s);
ast *newasgn(symbol *s, ast *v);
ast *newnum(double d);
ast *newflow(int nodetype, ast *cond, ast *tl, ast *tr);

/* 定义函数 */
void dodef(symbol *name, symlist *syms, ast *stmts);

/* 计算抽象语法树 */
double eval(ast *a);

/* 删除和释放抽象语法树 */
void treefree(ast *a);
