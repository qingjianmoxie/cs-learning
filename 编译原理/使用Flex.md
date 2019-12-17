# 使用Flex

## 正则表达式

`.`
    匹配除换行符(\n)之外的任意单一字符.
`[]`
    字符类(character class),可以匹配方括号中的任意一个字符. 如果字符类中的第一个字符是抑扬符号`^`,则改变为匹配除方括号内字符以外的任何字符. 字符类里的破折号表示字符的范围,例如,[a-z]表示任意小写字母.
`^`
    如果它是正则表达式的第一个字符就匹配行首. 它也被用于方括号中表示补集.
`$`
    如果它是正则表达式的最后一个字符就匹配行尾.
`{}`
    当花括号中带有一个或者两个数字时, 表示前一个模式可以匹配的最小和最大次数. 例如, A{1,3}匹配一到三个字母A, 而0{5}匹配00000. 当花括号中带有名字时, 它指向以这个名字命名的模式.
`\`
    用来表示元字符自身和一部分常用的C语言转义序列. 例如, `\n`表示换行符, `\*`表示字面意义上的星号.
`*`
    匹配零个或者多个紧接在前面的表达式. 例如, `[ \t]*`可以匹配任意多个空格和tab, 也就是空白字符.
`+`
    匹配一个或者多个紧接在前面的表达式. 例如, `[0-9]+`可以匹配数字字符串, 像1, 111或者123456, 但是不能是一个空字符串.
`?`
    匹配零个或者一个紧接在前面的表达式. 例如, `-?[0-9]+`匹配一个有符号数字, 他带有一个可选的前置负号.
`|`
    选择(alternative)操作符, 匹配紧接在前面的表达式或者紧跟在后面的表达式. 例如, faith|hope|charity匹配这三个美德中的任意一个.
`()`
    把一系列的正则表达式组成一个新的正则表达式. 例如, (01)匹配字符序列01, 而a(bc|de)匹配abc或者ade.

## flex如何处理二义性模式

大多数flex程序都具有二义性, 相同的输入可能被多种不同的模式匹配. flex通过两个简单的规则来解决它:
+ 词法分析器匹配输入时匹配尽可能多的字符串.
+ 如果两个模式都可以匹配的话, 匹配在程序中更早出现的模式.

## flex词法分析器中的文件I/O操作

我们现在修改第一章中的字数统计程序, 使他可以读取文件, 就像真正的wc程序所做的那样.
对于由flex和他的前身lex所生成的词法分析器来说, 除非你另作安排, 否则词法分析器总是通过名为yyin的文件句柄读取输入. 因此为了让他读取一个单一文件, 你只需要在第一次调用yylex之前重新设定yyin.
例2-1: 字数统计, 从文件中读取输入
```
/* 更像UNIX的wc程序 */
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
    if(argc > 1)
    {
        if(!(yyin = fopen(argv[1], "r")))
        {
            perror(argv[1]);
            return 1;
        }
    }
    yylex();
    printf("%8d%8d%8d\n", lines, words, chars);
    return 0;
}
```
与第一章例子的不同之处在于第三部分. 如果用户在命令行中给出了文件名, 主例程会打开这个文件并把相应的文件句柄赋给yyin; 否则, yyin将保持未赋值的状态, 这种情况下, yylex会自动把stdin赋给他.

## 读取多个文件
对于那些只要求从头到尾读完整个输入文件的程序来说, flex提供了yyrestart(f)例程, 他可以使词法分析器读取标准输入输出文件f.
例2-2: 字数统计, 读取多个文件
```
/* 读取多个文件 */
%option noyywrap
%{
int chars = 0;
int words = 0;
int lines = 0;

int totchars = 0;
int totwords = 0;
int totlines = 0;
%}
%%
[a-zA-Z]+  { words++; chars += strlen(yytext); }
\n         { chars++; lines++; }
.          { chars++; }
%%
int main(int argc, char **argv)
{
    if(argc < 2)    /* 读取标准输入 */
    {
        yylex();
        printf("%8d%8d%8d\n", lines, words, chars);
        return 0;
    }
    for(int i = 1; i < argc; i++)
    {
        FILE *f = fopen(argv[i], "r");
        if(!f)
        {
            perror(argv[i]);
            return 1;
        }
        yyrestart(f);
        yylex();
        fclose(f);
        printf("%8d%8d%8d %s\n", lines, words, chars, argv[i]);
        totchars += chars; chars = 0;
        totwords += words; words = 0;
        totlines += lines; lines = 0;
    }
    if(argc > 1)    /* 超过一个文件时打印出总数 */
        printf("%8d%8d%8d total\n", totlines, totwords, totchars);
    return 0;
}
```
