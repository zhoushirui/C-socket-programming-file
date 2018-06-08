/************************************************************************
	> File Name: file_client.c
	> Author: LiJin
	> Mail: 594268218@qq.com homtingli@gmail.com
	> Created Time: 2014-11-19 16:20
 ************************************************************************/
#include <iostream>
#include <netinet/in.h>                         //sockaddr_in结构的头文件
#include <sys/types.h>                          //socket的头文件
#include <sys/socket.h>                         //socket的头文件
#include <stdio.h>                              //printf（）
#include <stdlib.h>                             //exit()
#include <string.h>
#include <sys/stat.h>                           //bzero()函数
#include<unistd.h>                                //close,write,read函数
#include <arpa/inet.h>                           //inet_aton函数
#define SERVER_PORT       6600           //服务器的端口号
#define BUFFER_SIZE              1024    //缓冲区的长度
#define FILE_NAME_MAX_SIZE       512     //文件名的最大长度

long get_file_size( char * filename )//定义为long整型
{
    struct stat f_stat;//系统中文件属性的结构为f_stat
    if( stat( filename, &f_stat ) == -1 ) //将指针filename所指的文件结构（状态）复制到f_stat中，复制不成功返回-1
    {
        return -1;
    }
    return (long)f_stat.st_size;//若复制成功，返回该文件长度（为整型）
}
int main(int argc, char **argv)//int argc记录输入在命令行的字符串的个数，*argv指针数组，存放输入在命令行上的命令
{
    if (argc != 2)//只有参数等于2的时候才会正常向下进行，即一个客户端一个服务器端，否则退出
    {
        printf("Usage: %s ServerIPAddress\n", argv[0]);//argv[0]指向程序的全路径名
        exit(1);
    }
    
    // 设置一个socket地址结构client_addr, 代表客户机的internet地址和端口
    struct sockaddr_in client_addr;//sockaddr_in是一个包含互联网的结构地址 变量client_addr包含了客户端的地址
    bzero(&client_addr, sizeof(client_addr));//初始化client_addr，置客户端地址所占长度的字节都为0
    client_addr.sin_family = AF_INET; //变量client_addr包含了服务器的地址,sin_family包含地址族的代码。它应该始终设置为AF_INET套接字地址。
    client_addr.sin_addr.s_addr = htons(INADDR_ANY); // //INADDR_ANY包含主机的ip地址，也是client端机器永远的ip地址
    client_addr.sin_port = htons(0); //把主机字节顺序的端口号传递到网络字节顺序的端口号中，让系统自动分配一个空闲端口
    // 创建连续套接口
    // 创建用于internet的流协议(TCP)类型socket，用client_socket代表客户端socket
    int client_socket = socket(AF_INET, SOCK_STREAM, 0);//socket()函数创建一个新的套接字，它需要三个参数PF_INET：套接字的地址。SOCK_STREAM：套接字的类型。第三是协议论证：应该始终为0操作系统将选择最合适的协议。
    if (client_socket == -1)//如果套接字失败，socket返回-1
    {
        printf("Create Socket Failed!\n");//套接字创建失败
        exit(1);
    }
    // 把客户端的socket和客户端的socket地址结构绑定
    if (bind(client_socket, (struct sockaddr*)&client_addr, sizeof(client_addr)))
        //bind，系统调用将套接字绑定到地址，在这种情况下是当前主机和端口号的地址客户端将在其上运行。
        //套接字，套接字地址，套接字结构（长度）
    {
        printf("Client Bind Port Failed!\n");//绑定不成功
        exit(1);
    }
    struct sockaddr_in  server_addr; // 设置一个socket地址结构server_addr,代表服务器的internet地址和端口
    bzero(&server_addr, sizeof(server_addr));//初始化server_addr，置客户端地址所占长度的字节都为0
    server_addr.sin_family = AF_INET;//变量server_addr包含了服务器的地址,sin_family包含地址族的代码。它应该始终设置为AF_INET套接字地址。
    // 服务器的IP地址来自程序的参数， addr是将要用的新的ip地址结构
    if (inet_aton(argv[1], &server_addr.sin_addr) == 0)//inet_aton是计算机函数，将一个字符串IP地址转换为32位的网络序列地址，如果函数成功返回值不为0.输入地址不正确返回0
        //argv[1]参数string包含ASCII表示IP地址，指向DOS命令行中执行程序名后的第一个字符串即server_addr（ip）
    {
        printf("Server IP Address Error!\n");//服务器端的地址错误
        exit(1);
    }
    server_addr.sin_port = htons(SERVER_PORT);//把主机字节顺序的端口号传递到网络字节顺序的端口号中
    socklen_t server_addr_length = sizeof(server_addr);//输入服务器端的地址长度
    
    // 向服务器发起连接请求，连接成功后client_socket代表客户端和服务器端的一个socket连接
    if (connect(client_socket, (struct sockaddr*)&server_addr, server_addr_length) < 0)
        //connect，connect函数被客户端调用建立到服务器的连接。此处有三个参数，1.套接字文件描述符，2.想要连接的主机地址（包括端口号）此处的端口号是服务器的端口号，不是自己的端楼号，3.地址的大小
        //如果server值成功返回0，失败server值返回-1

    {
        printf("Can Not Connect To %s!\n", argv[1]);//不能连接到：服务器端的地址
        exit(1);
    }
    
    char name[FILE_NAME_MAX_SIZE + 1];//定义name数组类型，大小为FILE_NAME_MAX_SIZE定义的大小，身份
    char nameback[FILE_NAME_MAX_SIZE + 1];//定义nameback数组类型，大小为FILE_NAME_MAX_SIZE定义的大小，身份返回值
    bzero(name, sizeof(name));//bzero（）函数，先把name字节都置为0
    printf("Please Input your Name and number(default \"lijin\")\n");
    scanf("%s", name);//输入用于核对的用户名称
    write(client_socket,name,strlen(name));//把client_socket套接字参数所指的文件传送name长度的字节到name中

    if(read(client_socket,nameback,FILE_NAME_MAX_SIZE)==-1)//若server端身份匹配成功，返回身份返回类型为success。nameback中为服务器端new_server_socket套接字参数所指的文件传送nameback个长度的字节。
    {
        printf("Authentication failure\n");//server端身份匹配不成功
        exit(1);
    }
    if(!strcmp(nameback,"success"))//如果身份返回类型nameback为success
    {
        printf("Authentication Success\n");//匹配成功
    }
    else
    {
      printf("Authentication failure\n");//匹配不成功
        exit(1);
    }
    char file_name[FILE_NAME_MAX_SIZE + 1];//定义文件的名称，最大长度为FILE_NAME_MAX_SIZE
    bzero(file_name, sizeof(file_name));//把长度为file_name的file_name数组中的字节都置为0，初始化file_name数组
    printf("Please Input File Name On Server.\t");
    scanf("%s", file_name);//输入文件名称
    
    char buffer[BUFFER_SIZE];//设置一个缓冲区
    bzero(buffer, sizeof(buffer));//初始化缓冲区
    strncpy(buffer, file_name, strlen(file_name) > BUFFER_SIZE ? BUFFER_SIZE : strlen(file_name));
    // 向服务器发送buffer中的数据，此时buffer中存放的是客户端需要接收的文件的名字
    send(client_socket, buffer, BUFFER_SIZE, 0);//send（）向已连接的socket发送数据
    FILE *fp = fopen(file_name, "w");//打开并写文件file_name
    if (fp == NULL)
    {
        printf("File:\t%s Can Not Open To Write!\n", file_name);
        exit(1);
    }
    // 从服务器端接收数据到buffer中
    bzero(buffer, sizeof(buffer));
    int length = 0;
    while(length = recv(client_socket, buffer, BUFFER_SIZE, 0))//从缓冲区接收数据
    {
        printf("file_block_length = %d\n",length);//成功，返回实际读取到的BUFFER_SIZE的数据项的个数，失败则返回0

        if (length <= 0)//不成功
        {
            printf("Recieve Data From Server %s Failed!\n", argv[1]);
            break;
        }
        
        int write_length = fwrite(buffer, sizeof(char), length, fp);//从文件中读取内容
        printf("write_lenth = %d\n",write_length);
        if (write_length < length)//文件长度空间不够
        {
            printf("No enough room,File:\t%s Write Failed!\n", file_name);
            break;
        }
        bzero(buffer, BUFFER_SIZE);//初始化缓冲区
    }
    fclose(fp);//关闭文件
    if(get_file_size(file_name)==0)//没有该文件
    {
        printf("get_file_size(file_name)=%ld\n",get_file_size(file_name));
        printf("filesize=0 OR no such file,File:\t%s Write Failed!\n",file_name);
        printf("\nNow delete the Tempfile[ %s ] on your disk...\n", file_name);
        
        
        if(remove(file_name)==0)//重置文件名
            printf("delete Tempfile ok,you can try again\n\n");
        
    }
    else
    {
        printf("Recieve File:\t %s From Server[%s] Finished!\n", file_name, argv[1]);//文件传输完成
    }
    
    close(client_socket);// 传输完毕，关闭socket
    return 0;
}


