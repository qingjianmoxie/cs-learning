# flex和bison简介

## 词法分析和语法分析

在编译器分析程序源码语法的领域中，一个关键的想法是把分析工作分成两个部分：词法分析(lexical analysis或称scanning)和语法分析(syntax analysis或称parsing)。
简单来说，词法分析把输入分割成一个个有意义的词块，称为记号(token)；而语法分析则确定这些记号是如何彼此关联的。

## 我们的第一个Flex程序

Unix系统(也包括Linux这样的类Unix系统)自带了一个字数统计(word count)程序, 这个程序可以读入一个文件然后报告这个文件的行数、单词书和字符数.

flex使我们能够仅仅十几行就完成这个wc程序。
例1-1 字数统计
```
/* just like Unix wc */
%{
int chars = 0;
int words = 0;
int lines = 0;
%}
%%
[a-zA-Z]+  { words++; chars += strlen(yytext); }
\n         { chars++; lines++; }
.          { chars++; }
%%
int main(int argc, char **argv)
{
    yylex();
    printf("%8d%8d%8d\n", lines, words, chars);
    return 0;
}
```

这个flex程序包含三个部分，各个部分通过`%%`分割。第一部分包含声明和选项配置，第二部分是一系列的模式和动作，第三部分是会被拷贝到生成的词法分析器里面的c代码。

在声明部分，`%{`和`%}`之间的代码会被原样照抄到生成的C文件的开头部分。在这个例子中，它只是用来设定了行数、单词数和字符数的变量。

在第二部分，每个模式处在一行的开头处，接着是模式匹配时所需要执行的C代码。这儿的C代码是用`{}`括住的一行或者多行语句。(模式必须在行首出现，因为flex认为以空白开始的行都是代码而把它们照抄到生成的C程序中。)

这个程序只有三个模式。
- 第一个模式，`[a-zA-Z]+`用来匹配一个单词。在方括号里面的字符串是一种字符类(character class)，能够匹配任意一个大小写字母，而`+`这个符号表示匹配一个或者多个前面的字符类，也就是一连串的字母，或者说一个单词。相关的动作更新匹配过的单词和字符的个数。在任意一个flex的动作中，变量yytext总是被设为指向本次匹配的输入文本。在这个例子里，我们所需要关心的是有多少个字符，因此我们可以借助这个变量来统计字符数。
- 第二个模式, `\n`用来匹配换行符. 相关的动作更新行数和字符数.
- 最后的模式是一个点号，他在正则表达式中代表任意一个字符(与shell脚本中的`?`相似)。关联的动作更新字符数。

>也许聪明的你会问，如果一个点号代表所有的字符，难道他不会也匹配第一个模式所应该匹配的字母吗？答案是肯定的，但是flex棋高一着的地方是他总是选择更长的匹配，而且如果两个模式都匹配的话，flex会选择在程序里面首先出现的那个模式。这是一个精妙的设计，我们会发现他非常有用。

末尾的C代码是我们的主程序，他负责调用flex提供的词法分析例程yylex()，并输出结果。在没有任何其他改变的情况下，词法分析器将读取标准输入。

将代码保存为test.l，我们来运行一下：
```
# flex test.l
# gcc lex.yy.c -lfl
# ./a.out
```
程序以ctrl+D结束。
首先我们用flex来翻译我们的程序，flex在没有任何错误的情况下默默地完成了翻译。接着我们编译flex生成的C程序：lex.yy.c；将他与相应的flex库文件(-lfl)链接。
真正的wc程序对单词的定义和这儿相比有些微小的差别，他的定义是没有空白字符的字符串。一旦我们知道哪些是空白字符，我们只需要把匹配单词的那个模式替换为匹配没有空白字符的字符串：
`[^ \t\n\r\f\v]+  { words++; chars += strlen(yytext); }`
在字符类开始部分的符号`^`是指匹配任意一个不在字符类里面的字符，而符号`+`依然意味着匹配一个或者多个前面的模式。

### flex编译问题

如果flex编译文件时提示`/bin/ld: cannot find -lfl`, 原因是系统缺少libfl.a文件.

解决办法:
yum安装flex和flex-devel

`sudo yum install flex`

第一步比较容易成功

`sudo yum install flex-devel`

该步骤出现问题2：

安装flex-devel时报错：

No package flex-devel available

点[这里](http://www.rpmfind.net/linux/rpm2html/search.php?query=flex-devel)下载flex**对应版本**的flex-devel rpm包

`sudo yum install flex-devel-2.5.37-6.el7.x86_64.rpm`

## 让flex和bison协同工作

我们的第一个同时使用flex和bison的程序将是一个桌面计算器. 首先, 我们编写一个词法分析器, 接着我们编写一个语法分析器, 并把两者结合起来.

为了让事情变得简单, 我们的计算器只需要识别整数、基本算术运算符和一元绝对值操作符.

```
/* 一个简单的flex词法分析器 */
%%
"+"         { printf("PLUS\n"); }
"-"         { printf("MINUS\n"); }
"*"         { printf("TIMES\n"); }
"/"         { printf("DIVIDE\n"); }
"|"         { printf("ABS\n"); }
[0-9]+      { printf("NUMBER %s\n", yytext); }
\n          { printf("NEWLINE\n"); }
[ \t]       { /* 忽略空白字符 */ }
.           { printf("Mystery character %s\n", yytext); }
%%
```

前5个模式就是操作符本身, 用引号引起, 而目前的动作仅仅是打印出匹配的内容. 引号告诉flex使用引号内文本的原义, 而不是把它们解释成正则表达式.

第6个模式匹配一个整数. 这种方括号括起的模式[0-9]可以匹配任意一个数字, 接着的`+`这个符号表示匹配一个或者多个前面的项, 也就是由一个或者多个数字组成的字符串. 相关的动作利用词法分析器每次匹配后所设置的yytext来打印出匹配的字符串.

在这个简单的flex程序中, 我们并没有第三段的c代码. flex库文件(-lfl)提供了一个极小的主程序来调用词法分析器, 这对我们的例子来说足够了.

让我们试验一下这个词法分析器:

    $ flex test.l
    $ gcc lex.yy.c -lfl
    $ ./a.out
    12+34
    NUMBER 12
    PLUS
    NUMBER 34
    NEWLINE
    5 6 / 7q
    NUMBER 5
    NUMBER 6
    DIVIDE
    NUMBER 7
    Mystery character q
    NEWLINE
    ^D
    $

## 记号编号和记号值

当flex词法分析器返回一个记号流时, 每个记号实际上有两个组成部分, 记号编号(token number)和记号值(token's value). 这里的记号编号是一个较小的整数. 这个数字是随意确定的, 但是零值总是意味着文件的结束. 当bison创建一个语法分析器时, bison自动从258(这样可以避免与文字字符记号产生冲突, 后面会详细讨论)起指派每个记号编号, 并且创建一个包含这些编号定义的`.h`文件. 不过, 这次我们将手动定义一些编号:


## 文法与语法分析
语法分析器的任务是找出输入记号之间的关系。一种常见的关系表达方式就是语法分析树(parse tree)。

## 联合编译Flex和Bison程序
```
/* test.y */
/* 计算器的最简版本 */
%{
#include <stdio.h>
%}
/* declare tokens */
%token  NUMBER
%token  ADD SUB MUL DIV ABS
%token  OP CP
%token  EOL
%%
calclist: /* 空规则 */
    | calclist exp EOL { printf("= %d\n", $2); }
    ;

exp: factor { $$ = $1; }
    | exp ADD factor { $$ = $1 + $3; }
    | exp SUB factor { $$ = $1 - $3; }
    ;

factor: term { $$ = $1; }
    | factor MUL term { $$ = $1 * $3; }
    | factor DIV term { $$ = $1 / $3; }
    ;

term: NUMBER { $$ = $1; }
    | ABS term { $$ = $2 >= 0 ? $2 : -$2; }
    | OP exp CP { $$ = $2; }
%%
yyerror(const char *s)
{
    fprintf(stderr, "error: %s\n", s);
}
int main(int argc, char **argv)
{
    yyparse();
    return 0;
}
```

```
/* test.l */
/* 计算器词法分析器 */
%{
# include "test.tab.h"
%}
%%
"+"         { return ADD; }
"-"         { return SUB; }
"*"         { return MUL; }
"/"         { return DIV; }
"|"         { return ABS; }
[0-9]+      { yylval = atoi(yytext); return NUMBER; }
\n          { return EOL; }
[ \t]       { /* 忽略空白字符 */ }
"("         { return OP; }
")"         { return CP; }
"//".*      { /* 忽略注释 */ }
.           { printf("Mystery character %c\n", *yytext); }
%%
```

现在的编译过程已经足够复杂，我们最好把它放到一个Makefile中：
```makefile
test: test.l test.y
    bison -d test.y
    flex test.l
    gcc -o $@ test.tab.c lex.yy.c -lfl
```
首先他以`-d`(用于"定义"文件)标志运行bison,这会创建test.tab.c和test.tab.h,接着他运行flex来创建lex.yy.c,然后它把两者和flex的库文件编译在一起.
