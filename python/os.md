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
