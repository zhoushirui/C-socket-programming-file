#include <iostream>
#include <netinet/in.h>//sockaddr_in结构的头文件
#include <sys/types.h>//socket的头文件
#include <sys/socket.h>//socket的头文件
#include <stdio.h>//printf（）
#include <stdlib.h>//exit()
#include <string.h> //bzero()函数
#include <fstream>//fp文件
#include <unistd.h>//read,write,close头文件

#define MY_SERVER_PORT    6600  //服务器的端口号
#define LENGTH_OF_LISTEN_QUEUE     20     //可监听队列长度
#define BUFFER_SIZE                1024    //缓冲区的长度
#define FILE_NAME_MAX_SIZE         512    //文件名的最大长度
#define NAME "lijin"
int main(int argc, char **argv)
{
    // 设置套接字的地址信息
    // 设置一个socket地址结构server_addr,代表服务器internet的地址和端口
    struct sockaddr_in   server_addr;//sockaddr_in是一个包含互联网的结构地址 变量server_addr包含了服务器的地址
    bzero(&server_addr, sizeof(server_addr));//bzero函数，置服务器地址所占长度的字节都为0
    server_addr.sin_family = AF_INET;//变量server_addr包含了服务器的地址,sin_family包含地址族的代码。它应该始终设置为AF_INET套接字地址。

    server_addr.sin_addr.s_addr = htons(INADDR_ANY);//INADDR_ANY包含主机的ip地址，也是运行服务器的机器的永远ip地址。
    server_addr.sin_port = htons(MY_SERVER_PORT);//把主机字节顺序的端口号传递到网络字节顺序的端口号中
    
    // 创建连续套接字
    // 创建用于internet的流协议(TCP)socket，用server_socket代表服务器向客户端提供服务的接口
    int server_socket = socket(PF_INET, SOCK_STREAM, 0);//socket()函数创建一个新的套接字，它需要三个参数PF_INET：套接字的地址。SOCK_STREAM：套接字的类型。第三是协议论证：应该始终为0操作系统将选择最合适的协议。
    if (server_socket < 0)//如果套接字失败，返回-1
    {
        printf("Create Socket Failed!\n");//套接字创建失败，socket返回-1
        exit(1);
    }
    else
    {
        printf("Create Socket success\n");//反之，套接字创建成功
    }
    
    // 把服务器端的socket和socket地址结构绑定
    if (bind(server_socket, (struct sockaddr*)&server_addr, sizeof(server_addr)))//bind，系统调用将套接字绑定到地址，在这种情况下是当前主机和端口号的地址服务器将在其上运行。
        //套接字，套接字地址，套接字结构（长度）
    {
        printf("Server Bind Port: %d Failed!\n", MY_SERVER_PORT);//绑定不成功
        exit(1);
    }
    else
    {
        printf("Server Bind Port: %d success!\n",MY_SERVER_PORT);//绑定成功，输出服务器端口号
    }
    
    // server_socket用于监听
    if (listen(server_socket, LENGTH_OF_LISTEN_QUEUE))//listen(),监听系统调用允许在套接字链接上进行监听，如果没有监听到则闲置。
        //server_socket为套接字文字描述符，LENGTH_OF_LISTEN_QUEUE为客户数量的大小即服务器可以连接的数量既监听队列的长度。
    {
        printf("Server Listen Failed!\n");//服务器监听调用不成功
        exit(1);
    }
    else
    {
        printf("Now listening\n");//服务器可以正常监听
    }
        // 监听活动在服务器端一直运行用以持续为客户端提供服务
    while(1)
    {
        // 接受此请求，同时将client端的地址和端口等信息写入client_addr中
        struct sockaddr_in client_addr;// 定义客户端的socket地址结构client_addr，当收到来自客户端的请求后，调用accept
        socklen_t      length = sizeof(client_addr);//length为socket地址长度
        // 接受一个从client端到达server端的连接请求,将客户端的信息保存在client_addr中
        // 用select()来实现超时检测
        int new_server_socket = accept(server_socket, (struct sockaddr*)&client_addr, &length); // accpet返回一个新的socket,这个socket用来与此次连接到server的client进行通信
        // 这里的new_server_socket代表了这个通信通道，客户端在此返回一个新的文件描述符，并且在此次连接上的所有通信都使用新的文件描述符。
         //(struct sockaddr *)&server_addr是另一个客户端的指针指向连接结束
        //accept（）函数调用会导致进程阻塞直到客户端有连接请求连接到服务器。
        if (new_server_socket < 0)//检查返回值accept（）是否有效
        {
            printf("Server Accept Failed!\n");//若无效
            break;
        }
        
        char name[BUFFER_SIZE];//定义name数组类型，大小为buffer区大小，身份
        char nameback[BUFFER_SIZE];//定义nameback数组类型，身份返回值
        bzero(name, sizeof(name));// bzero（）函数，先把name字节都置为0
        read(new_server_socket,name,BUFFER_SIZE);//把new_server_socket套接字参数所指的文件传送BUFFER_SIZE个字节到name中
        printf("%s",name);//输出从client端传回的新的socket，也就是身份name
        if (strcmp(name,NAME))        //把从client端传回的name和之前定义好的NAME相比较
        {
            strcpy(name,"error");//输入的身份名称不匹配
            write(new_server_socket,name,strlen(name));//把new_server_socket套接字参数所指的文件传送name长度的字节到name中
            printf("\tAuthentication failure\n");//name不匹配
            continue;//继续
        }
        else
        {
            printf("\tAuthentication success\n");//身份匹配成功
            strcpy(nameback,"success");//身份返回类型为success
            write(new_server_socket,nameback,strlen(nameback));//把new_server_socket套接字参数所指的文件传送nameback个长度的字节到nameback中
        }
        
        char buffer[BUFFER_SIZE];//定义缓冲区的长度为BUFFER_SIZE的缓冲区
        bzero(buffer, sizeof(buffer));//把长度为buffer的buffer缓冲区的字节都置为0
        length = recv(new_server_socket, buffer, BUFFER_SIZE, 0);//recv函数是从TCP连接的另一端接收数据，接收buffer数据缓冲区中字节数为BUFFER_SIZE的的套接字描述符到server端，返回值为length
          //即从client端接收文件名称
        if (length < 0)//若recv（）函数返回值小于0
        {
            printf("Server Recieve Data Failed!\n");//服务器端接收数据失败
            break;//中断程序
        }
        
        char file_name[FILE_NAME_MAX_SIZE + 1];//定义文件的名称，最大长度为FILE_NAME_MAX_SIZE
        bzero(file_name, sizeof(file_name));//把长度为file_name的file_name数组中的字节都置为0，初始化file_name数组
        strncpy(file_name, buffer,
                strlen(buffer) > FILE_NAME_MAX_SIZE ? FILE_NAME_MAX_SIZE : strlen(buffer));//把buffer中的FILE_NAME_MAX_SIZE或strlen(buffer)中较长的那个的字节复制到file_name中去，即把client输入到buffer中的文件名称输入到file_name中去
        FILE *fp = fopen(file_name, "r");//打开并读文件file_name
        if (fp == NULL)//如果文件为空
        {
            printf("File:\t%s Not Found!\n", file_name);//该文件未找到
        }
        else//若文件不为空
        {
            bzero(buffer, BUFFER_SIZE);//把长度为BUFFER_SIZE的buffer缓冲区的字节都置为0
            int file_block_length = 0;//定义变量
            /*if((file_block_length = fread(buffer, sizeof(char), BUFFER_SIZE, fp)) == 0)
             {
             printf("File:\t%s Not Found!\n", file_name);
             continue;
             }
             else*/
            while( (file_block_length = fread(buffer, sizeof(char), BUFFER_SIZE, fp)) > 0)//从fp中输入BUFFER_SIZE个数据项每个数据项为char的长度个字节的数据进入buffer缓冲区中
            {
                printf("file_block_length = %d\n", file_block_length);//成功，返回实际读取到的BUFFER_SIZE的数据项的个数，失败则返回0
                if (send(new_server_socket, buffer, file_block_length, 0) <= 0)//若发送不成功
                    // 发送buffer中的字符串长度为file_block_length到new_server_socket,实际上就是发送给客户端
                {
                    printf("Send File:\t%s Failed!\n", file_name);//文件发送失败
                    break;
                }
                
                bzero(buffer, sizeof(buffer));//buffer缓冲区的内容清空，重置为0
            }
            fclose(fp);//关闭文件
            printf("File:\t%s Transfer Finished!\n", file_name);//文件传输成功
        }
        
        close(new_server_socket);//关闭名为new_server_socket的socket
    
        close(server_socket);//关闭名为server_socket的socket
    
    return 0;
}
