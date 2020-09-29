# socket编程实现文件传输功能

这节我们来完成 socket 文件传输程序，这是一个非常实用的例子。要实现的功能为：client 从 server 下载一个文件并保存到本地。

编写这个程序需要注意两个问题：
1) 文件大小不确定，有可能比缓冲区大很多，调用一次 write()/send() 函数不能完成文件内容的发送。接收数据时也会遇到同样的情况。

要解决这个问题，可以使用 while 循环，例如：
```c++
//Server 代码
int nCount;
while( (nCount = fread(buffer, 1, BUF_SIZE, fp)) > 0 ){
    send(sock, buffer, nCount, 0);
}
//Client 代码
int nCount;
while( (nCount = recv(clntSock, buffer, BUF_SIZE, 0)) > 0 ){
    fwrite(buffer, nCount, 1, fp);
}
```

对于 Server 端的代码，当读取到文件末尾，fread() 会返回 0，结束循环。

对于 Client 端代码，有一个关键的问题，就是文件传输完毕后让 recv() 返回 0，结束 while 循环。

> 注意：读取完缓冲区中的数据 recv() 并不会返回 0，而是被阻塞，直到缓冲区中再次有数据。

2) Client 端如何判断文件接收完毕，也就是上面提到的问题——何时结束 while 循环。

最简单的结束 while 循环的方法当然是文件接收完毕后让 recv() 函数返回 0，那么，如何让 recv() 返回 0 呢？**recv() 返回 0 的唯一时机就是收到FIN包时**。

FIN 包表示数据传输完毕，计算机收到 FIN 包后就知道对方不会再向自己传输数据，当调用 read()/recv() 函数时，如果缓冲区中没有数据，就会返回 0，表示读到了”socket文件的末尾“。

这里我们调用 shutdown() 来发送FIN包：server 端直接调用 close()/closesocket() 会使输出缓冲区中的数据失效，文件内容很有可能没有传输完毕连接就断开了，而调用 shutdown() 会等待输出缓冲区中的数据传输完毕。

本节以Windows为例演示文件传输功能，Linux与此类似，不再赘述。请看下面完整的代码。

服务器端 server.cc:
windows
```c++
#include <stdio.h>
#include <stdlib.h>
#include <winsock2.h>
#pragma comment (lib, "ws2_32.lib")  //加载 ws2_32.dll
#define BUF_SIZE 1024
int main(){
    //先检查文件是否存在
    char *filename = "D:\\send.avi";  //文件名
    FILE *fp = fopen(filename, "rb");  //以二进制方式打开文件
    if(fp == NULL){
        printf("Cannot open file, press any key to exit!\n");
        system("pause");
        exit(0);
    }
    WSADATA wsaData;
    WSAStartup( MAKEWORD(2, 2), &wsaData);
    SOCKET servSock = socket(AF_INET, SOCK_STREAM, 0);
    sockaddr_in sockAddr;
    memset(&sockAddr, 0, sizeof(sockAddr));
    sockAddr.sin_family = PF_INET;
    sockAddr.sin_addr.s_addr = inet_addr("127.0.0.1");
    sockAddr.sin_port = htons(1234);
    bind(servSock, (SOCKADDR*)&sockAddr, sizeof(SOCKADDR));
    listen(servSock, 20);
    SOCKADDR clntAddr;
    int nSize = sizeof(SOCKADDR);
    SOCKET clntSock = accept(servSock, (SOCKADDR*)&clntAddr, &nSize);
    //循环发送数据，直到文件结尾
    char buffer[BUF_SIZE] = {0};  //缓冲区
    int nCount;
    while( (nCount = fread(buffer, 1, BUF_SIZE, fp)) > 0 ){
        send(clntSock, buffer, nCount, 0);
    }
    shutdown(clntSock, SD_SEND);  //文件读取完毕，断开输出流，向客户端发送FIN包
    recv(clntSock, buffer, BUF_SIZE, 0);  //阻塞，等待客户端接收完毕
    fclose(fp);
    closesocket(clntSock);
    closesocket(servSock);
    WSACleanup();
    system("pause");
    return 0;
}
```

linux
```c++
#include <arpa/inet.h>
#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <unistd.h>

#define BUF_SIZE 1024

int main() {
    //先检查文件是否存在
    const char *filename = "/home/jiangjie/test.py";  //文件名
    FILE *fp = fopen(filename, "rb");                 //以二进制方式打开文件
    if (fp == NULL) {
        printf("Cannot open file, press any key to exit!\n");
        return 0;
    }

    //创建套接字
    int serv_sock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);

    //绑定套接字
    struct sockaddr_in serv_addr;
    memset(&serv_addr, 0, sizeof(serv_addr));            //每个字节都用0填充
    serv_addr.sin_family = AF_INET;                      //使用IPv4地址
    serv_addr.sin_addr.s_addr = inet_addr("127.0.0.1");  //具体的IP地址
    serv_addr.sin_port = htons(1234);                    //端口
    bind(serv_sock, (struct sockaddr *)&serv_addr, sizeof(serv_addr));

    //进入监听状态
    listen(serv_sock, 20);

    //接收客户端请求
    struct sockaddr_in clnt_addr;
    socklen_t clnt_addr_size = sizeof(clnt_addr);
    int clnt_sock = accept(serv_sock, (struct sockaddr *)&clnt_addr, &clnt_addr_size);
    //循环发送数据，直到文件结尾
    char buffer[BUF_SIZE] = {0};  //缓冲区
    int nCount;
    while ((nCount = fread(buffer, 1, BUF_SIZE, fp)) > 0) {
        write(clnt_sock, buffer, nCount);
    }
    shutdown(clnt_sock, SHUT_WR);       //文件读取完毕，断开输出流，向客户端发送FIN包
    read(clnt_sock, buffer, BUF_SIZE);  //阻塞，等待客户端接收完毕
    fclose(fp);
    //关闭套接字
    close(clnt_sock);
    close(serv_sock);
    return 0;
}
```

客户端代码：
windows
```c++
#include <stdio.h>
#include <stdlib.h>
#include <WinSock2.h>
#pragma comment(lib, "ws2_32.lib")
#define BUF_SIZE 1024
int main(){
    //先输入文件名，看文件是否能创建成功
    char filename[100] = {0};  //文件名
    printf("Input filename to save: ");
    gets(filename);
    FILE *fp = fopen(filename, "wb");  //以二进制方式打开（创建）文件
    if(fp == NULL){
        printf("Cannot open file, press any key to exit!\n");
        system("pause");
        exit(0);
    }
    WSADATA wsaData;
    WSAStartup(MAKEWORD(2, 2), &wsaData);
    SOCKET sock = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP);
    sockaddr_in sockAddr;
    memset(&sockAddr, 0, sizeof(sockAddr));
    sockAddr.sin_family = PF_INET;
    sockAddr.sin_addr.s_addr = inet_addr("127.0.0.1");
    sockAddr.sin_port = htons(1234);
    connect(sock, (SOCKADDR*)&sockAddr, sizeof(SOCKADDR));
    //循环接收数据，直到文件传输完毕
    char buffer[BUF_SIZE] = {0};  //文件缓冲区
    int nCount;
    while( (nCount = recv(sock, buffer, BUF_SIZE, 0)) > 0 ){
        fwrite(buffer, nCount, 1, fp);
    }
    puts("File transfer success!");
    //文件接收完毕后直接关闭套接字，无需调用shutdown()
    fclose(fp);
    closesocket(sock);
    WSACleanup();
    system("pause");
    return 0;
}
```

linux
```c++
#include <arpa/inet.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <unistd.h>

#define BUF_SIZE 1024
int main() {
    //先输入文件名，看文件是否能创建成功
    char filename[100] = {0};  //文件名
    printf("Input filename to save: ");
    gets(filename);
    FILE *fp = fopen(filename, "wb");  //以二进制方式打开（创建）文件
    if (fp == NULL) {
        printf("Cannot open file, press any key to exit!\n");
        return 0;
    }

    //创建套接字
    int sock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    struct sockaddr_in serv_addr;
    memset(&serv_addr, 0, sizeof(serv_addr));            //每个字节都用0填充
    serv_addr.sin_family = AF_INET;                      //使用IPv4地址
    serv_addr.sin_addr.s_addr = inet_addr("127.0.0.1");  //具体的IP地址
    serv_addr.sin_port = htons(1234);                    //端口

    //向服务器发起请求
    connect(sock, (struct sockaddr *)&serv_addr, sizeof(serv_addr));
    //循环接收数据，直到文件传输完毕
    char buffer[BUF_SIZE] = {0};  //文件缓冲区
    int nCount;
    while ((nCount = read(sock, buffer, BUF_SIZE)) > 0) {
        fwrite(buffer, nCount, 1, fp);
    }
    puts("File transfer success!");
    //文件接收完毕后直接关闭套接字，无需调用shutdown()
    fclose(fp);
    close(sock);
    return 0;
}
```

在D盘中准备好send.avi文件，先运行 server，再运行 client：
Input filename to save: D:\\recv.avi↙
//稍等片刻后
File transfer success!

打开D盘就可以看到 recv.avi，大小和 send.avi 相同，可以正常播放。

注意 server.cpp 第42行代码，recv() 并没有接收到 client 端的数据，当 client 端调用 closesocket() 后，server 端会收到FIN包，recv() 就会返回，后面的代码继续执行。
