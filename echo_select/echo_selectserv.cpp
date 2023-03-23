// 使用select实现i/o多路复用echo服务器端
#include<iostream>
#include<sys/socket.h>
#include<sys/select.h>
#include<sys/time.h>
#include<arpa/inet.h>
#include<unistd.h>
using namespace std;
#define BUF_SIZE 100

int main()
{
    int serv_socket;
    int clnt_socket;
    struct sockaddr_in serv_addr,clnt_addr;
    struct timeval timeout; // select 超时时间
    fd_set reads , cpy_reads; // select 文件描述符数组
    int fd_max = 0; // 数组最大值
    int fd_num = 0; // 记录select返回值
    
    char buf[BUF_SIZE];

    serv_socket = socket(PF_INET,SOCK_STREAM,0);
    memset(&serv_addr,0,sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_addr.s_addr = htonl(INADDR_ANY);
    serv_addr.sin_port = htons(8888);

    if(bind(serv_socket,(struct sockaddr*)&serv_addr,sizeof(serv_addr)) == -1)
    {
        cout << "bind error\n" << endl;
        exit(1);
    }
        
    if(listen(serv_socket,5) == -1)
    {
        cout << "listen error\n" << endl;
        exit(1);
    }

    FD_ZERO(&reads);
    FD_SET(serv_socket,&reads); // 注册服务端套接字
    fd_max = serv_socket;


    while(1)
    {
        cpy_reads = reads; // 每执行一次循环，cpy_reads的值会改变，所以要在一开始重新赋值
        timeout.tv_sec = 5;
        timeout.tv_usec = 5000; // 微秒

        if((fd_num = select(fd_max+1,&cpy_reads,0,0,&timeout)) == -1)
            break;
        if(fd_num == 0) continue;

        for(int i = 0 ; i <fd_max+1; i++)
        {
            if(FD_ISSET(i,&cpy_reads)) // 查找出所有发生变化的文件描述符
            {
                if(i == serv_socket) // 发生变化的文件描述符为服务端套接字
                {
                    socklen_t adr_sz = sizeof(clnt_addr); // todo
                    clnt_socket = accept(serv_socket,(struct sockaddr*) &clnt_addr,&adr_sz);

                    FD_SET(clnt_socket,&reads);
                    if(fd_max < clnt_socket)
                    {
                        fd_max = clnt_socket;
                    }
                    cout << "Connected client: " << clnt_socket << endl;
                }
                else // 发生变化的文件描述符不为服务端套接字，则表示是其他的客户端套接字
                {
                    int len = recv(i,buf,BUF_SIZE,0);
                    if(len  == 0) 
                    {
                        FD_CLR(i,&reads);
                        close(i);
                        cout << "\nClosed client :" << i << endl;
                    }
                    else
                    {
                        cout << "\n[get buffer from " << i << "]";
                        for(int i = 0 ;i < len;i++)
                        {
                            cout << buf[i] << " ";
                        }
                        send(i,buf,len,0);
                    }
                }
            }
        }

    }
    close(serv_socket);
    return 0;
}
