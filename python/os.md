# os模块

## os.listdir()

### 概述

os.listdir() 方法用于返回指定的文件夹包含的文件或文件夹的名字的列表。

它不包括 `.` 和 `..` 即使它在文件夹中。

只支持在 Unix, Windows 下使用。

### 语法

listdir()方法语法格式如下：
```py
os.listdir(path)
```

### 参数

+ path -- 需要列出的目录路径

### 返回值

返回指定路径下的文件和文件夹列表。

### 实例

以下实例演示了 listdir() 方法的使用：

```py
#!/usr/bin/python
# -*- coding: UTF-8 -*-

import os, sys

# 打开文件
path = "/home/jiangjie/"
dirs = os.listdir(path)

# 输出所有文件和文件夹
for file in dirs:
   print file
```

执行以上程序输出结果为：

    test.txt
    robots.txt
    itemlisting
    resumelisting
    advertisebusiness.htm
    papers
    resume

如果目录下有中文目录，打印时遇到乱码解决方法：
```py
cPath = os.getcwd()

# 如果目录名字为中文 需要转码处理
uPath = unicode(cPath,'utf-8')
for fileName in os.listdir(uPath) :
    print fileName
```

## os.path()

os.path 模块主要用于获取文件的属性。

以下是 os.path 模块的几种常用方法：

| 方法 | 说明 |
| :- | :- |
| os.path.abspath(path)                | 返回绝对路径 |
| os.path.basename(path)               | 返回文件名 |
| os.path.commonprefix(list)           | 返回list(多个路径)中，所有path共有的最长的路径 |
| os.path.dirname(path)                | 返回文件路径 |
| os.path.exists(path)                 | 如果路径 path 存在，返回 True；如果路径 path 不存在，返回 False。 |
| os.path.lexists                      | 路径存在则返回True,路径损坏也返回True |
| os.path.expanduser(path)             | 把path中包含的"~"和"~user"转换成用户目录 |
| os.path.expandvars(path)             | 根据环境变量的值替换path中包含的"$name"和"${name}" |
| os.path.getatime(path)               | 返回最近访问时间（浮点型秒数） |
| os.path.getmtime(path)               | 返回最近文件修改时间 |
| os.path.getctime(path)               | 返回文件 path 创建时间 |
| os.path.getsize(path)                | 返回文件大小，如果文件不存在就返回错误 |
| os.path.isabs(path)                  | 判断是否为绝对路径 |
| os.path.isfile(path)                 | 判断路径是否为文件 |
| os.path.isdir(path)                  | 判断路径是否为目录 |
| os.path.islink(path)                 | 判断路径是否为链接 |
| os.path.ismount(path)                | 判断路径是否为挂载点 |
| os.path.join(path1[, path2[, ...]])  | 把目录和文件名合成一个路径 |
| os.path.normcase(path)               | 转换path的大小写和斜杠 |
| os.path.normpath(path)               | 规范path字符串形式 |
| os.path.realpath(path)               | 返回path的真实路径 |
| os.path.relpath(path[, start])       | 从start开始计算相对路径 |
| os.path.samefile(path1, path2)       | 判断目录或文件是否相同 |
| os.path.sameopenfile(fp1, fp2)       | 判断fp1和fp2是否指向同一文件 |
| os.path.samestat(stat1, stat2)       | 判断stat tuple stat1和stat2是否指向同一个文件 |
| os.path.split(path)                  | 把路径分割成 dirname 和 basename，返回一个元组 |
| os.path.splitdrive(path)             | 一般用在 windows 下，返回驱动器名和路径组成的元组 |
| os.path.splitext(path)               | 分割路径，返回路径名和文件扩展名的元组 |
| os.path.splitunc(path)               | 把路径分割为加载点与文件 |
| os.path.supports_unicode_filenames   | 设置是否支持unicode路径名 |

实例

```py
#!/usr/bin/python
# -*- coding: UTF-8 -*-

import os

print(os.path.join('root','jiangjie','test.txt'))  # 将目录和文件名合成一个路径
```

输出结果为:

    root/jiangjie/test.txt
