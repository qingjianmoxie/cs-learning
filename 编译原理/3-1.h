/* 3-1.h */
/* 3-1计算器的声明部分 */

/* 与词法分析器的接口 */
extern int yylineno;
void yyerror(const char *s, ...);

/* 抽象语法树中的节点 */
typedef struct ast
{
    int nodetype;
    struct ast *l;
    struct ast *r;
} ast;

typedef struct numval
{
    int nodetype; /* 类型K 表明常量 */
    double number;
} numval;

/* 构造抽象语法树 */
ast *newast(int nodetype, ast *l, ast *r);
ast *newnum(double d);

/* 计算抽象语法树 */
double eval(ast *a);

/* 删除和释放抽象语法树 */
void treefree(ast *a);
