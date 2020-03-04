|命令|说明|
|:-|:-|
|ps -ef&#124;grep xxx|查看进程号|
|gdb -p xxxx| |
|b function|打断点（通过函数名）|
|b filename:linenum|打断点（通过文件名:行号）|
|c|continue|
|p valuename|查看内容|
|info break|查看断点|
|disable 断点编号|使该断点失效|
|enable 断点编号|使该断点生效|
|enable once 断点编号|该断点只启用一次，程序运行到该断点并暂停后，该断点即被禁用|
|delete 断点编号（多个断点编号用空格隔开）|删除断点|
|n|next单步向下执行|
|s|(step)单步执行，且可以进入函数|
|bt|打印全部堆栈信息|
|finish|退出函数|

## gdb取消显示长度限制
set print elements 0