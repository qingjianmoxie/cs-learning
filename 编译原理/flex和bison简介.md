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

    NUMBER = 258
    ADD = 259
    SUB = 260
    MUL = 261
    DIV = 262
    ABS = 263
    EOL = 264 行结束

(当然, 事实上, 正如我们后面会看到的, 它们也就是bison所创建的记号编号列表. 但是这些编号一样好用.)

```
/* 计算器词法分析器 */
%{
    enum yytokentype {
        NUMBER = 258,
        ADD = 259,
        SUB = 260,
        MUL = 261,
        DIV = 262,
        ABS = 263,
        EOL = 264,
    };
    int yylval;
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
.           { printf("Mystery character %c\n", *yytext); }
%%
int main() {
    int tok;
    while (tok = yylex()) {
        printf("%d", tok);
        if (NUMBER == tok) {
            printf(" = %d\n", yylval);
        } else {
            printf("\n");
        }
    }
    return 0;
}
```

测试结果如下:

    a / 34 + |45
    Mystery character a
    262
    258 = 34
    259
    263
    258 = 45
    264

现在我们有了一个可以工作的词法分析器, 我们可以把注意力转向语法分析器啦.

## 文法与语法分析

语法分析器的任务是找出输入记号之间的关系。一种常见的关系表达方式就是语法分析树(parse tree)。例如, 算术表达式 1 * 2 + 3 * 4 + 5 有如下语法分析树:

```
          +
         / \
        +   5
       / \
      *   *
     / \ / \
    1  2 3  4 
```

这颗树的结构十分简单和普通, 每个节点都有两个儿子节点(这也是我们使用计算器作为第一个例子的原因), 但是每一个bison语法分析器在分析其输入时, 都会构造一颗语法分析树. 在有些应用中, 它把整棵树作为一个数据结构创建在内存中以便后续使用. 在其他应用中, 语法分析树只是隐式地包含在语法分析器进行的一系列操作中.

## bison的规则描述语言

bison的规则基本上就是BNF, 但是做了一点点简化以易于输入. 下面实现我们的第一版计算器的bison代码:

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
int main() {
    yyparse();
    return 0;
}
```

bison程序包含了与flex程序相同的三部分结构(不是巧合): 声明部分、规则部分和C代码部分. 
1. 声明部分包含了会被原样拷贝到目标分析程序开头的C代码, 同样也通过`%{`和`%}`来声明. 随后是`%token`记号声明, 以便于告诉bison在语法分析器中记号的名称. 记号通常使用大写字母, 虽然bison并不强制这么做. 任何没有声明为记号的语法符号必须出现在至少一条规则的左边. (如果一个语法符号既不是记号也没有出现在任何规则的左边, 它就像c程序里没有被引用的变量. 它虽然没有破坏任何事情, 但是可能意味着程序员犯了个错误.)
2. 规则部分使用`:`而不是`::=`, 同时由于行间隔不那么明显, 使用分号来表示规则的结束. 和flex一样, C的动作代码在每条规则之后用花括号括起.
每个bison规则中的语法符号都有一个语义值, 目标符号(冒号左边的语法符号)的值在动作代码中用`$$`代替, 右边语法符号的语义值依次为`$1`、`$2`, 直到这条规则的结束

## 联合编译Flex和Bison程序

在我们把词法分析器和语法分析器构造为同一个可以工作的程序前, 我们需要对之前的词法分析器做一些小小的改动. 具体就是我们包含bison创建的头文件, 而不是在第一部分里定义显式的记号值. 那个头文件包含了记号编号的定义和yylval的定义. 同时删除了词法分析器第三部分的代码, 因为语法分析器会调用词法分析器.

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

## 二义性文法

也许你会想上面的文法是否必须这么复杂, 为什么不写成这样呢?

```
exp: exp ADD exp
    | exp SUB exp
    | exp MUL exp
    | exp DIV exp
    | ABS exp
    | OP exp CP
    | NUMBER
    ;
```

有两个原因: 优先级和二义性. 分开的term、factor和exp语法符号可以让bison首先处理ABS和括号, 接着处理MUL和DIV, 然后是ADD和SUB. 通常, 一旦一种文法有不同优先级, 语法分析器就需要为每种优先级制定一条规则.

既然如此, 下面的文法又如何呢?

```
exp: exp ADD exp
    | exp SUB exp
    | factor
    ;

factor: factor MUL factor
    | factor DIV factor
    | term
    ;

term: ABS term
    | OP term CP
    | NUMBER
    ;
```

bison最强大的地方之一, 同时也是它最令人烦恼的特征之一, 就是它不分析二义性文法. 前面的文法是有歧义的, 因为类似 1 - 2 + 3 的输入可以分析为 (1-2)+3 或者 1-(2+3), 两种表达式的结果也不同.

如果一种文法是有歧义的, bison会报告冲突(conflicts), 并且标示出针对给定的输入哪儿会有两种不同的分析. bison还是会创建一种语法分析器, 它会选择冲突中的一种分析方式, 但是这种选择不一定是你想指定的那种.

bison常用的分析算法可以向前看一个记号来决定用哪种规则来匹配输入. 有的文法并没有二义性, 但是有些地方可能需要向前看更多的记号来决定匹配的规则. 这也可能导致冲突, 通常可以重写文法使得只要向前看一个记号就够了.

为每个优先级分开编写规则是一件乏味的事情, 事实上, 可以事先告诉bison优先级和分组的规则以便解决二义性.
