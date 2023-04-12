/*
对echo_mpserv进行改进，添加功能：
    将回声客户端传输的字符串按序保存到文件中
为了实现该任务：
    创建一个新进程，专门从为客户端提供回声服务的进程中读取字符信息。
    使用管道单向通信
*/

#include<iostream>
#include<sys/socket.h>
#include <arpa/inet.h>
#include<unistd.h>
#include<sys/wait.h>
#include<signal.h>
using namespace std;

void read_childproc(int sig)
{
    int status;
    pid_t pid = waitpid(-1,&status,WNOHANG);
    if(WIFEXITED(status))
    {
        cout << "Removed proc id :" << pid << endl;
        cout << "Child send:" << WEXITSTATUS(status) << endl;
    }
}

int main()
{
    int serv_socket;
    int clnt_socket;
    struct sockaddr_in serv_addr;
    struct sockaddr_in clnt_addr;
    socklen_t clnt_addr_len = sizeof(clnt_addr);
    int fds[2];
    pid_t pid;

    struct sigaction act;
    act.sa_handler = read_childproc;
    sigemptyset(&act.sa_mask);
    act.sa_flags = 0;
    sigaction(SIGCHLD,&act,0);

    serv_socket = socket(PF_INET,SOCK_STREAM,0);
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_addr.s_addr = htonl(INADDR_ANY); //0.0.0.0 表示不确定地址，或所有地址、任意地址
    serv_addr.sin_port = htons(8888);

    bind(serv_socket,(sockaddr *)&serv_addr,sizeof(serv_addr));
    listen(serv_socket,5);

    pipe(fds);
    pid = fork();
    if(pid == 0)
    {
        FILE *fp = fopen("echomsg.txt","a+");
        char msgbuf[BUFSIZ];
        int len;
        for(int i = 0 ; i < 10 ; i++) // 一次保存10个信息
        {
            len = read(fds[0],msgbuf,BUFSIZ);
            fwrite((void *)msgbuf,1,len,fp);
        }
        fclose(fp);
        return 0;   
    }

    while(1)
    {
        clnt_socket = accept(serv_socket,(sockaddr *)&clnt_addr,&clnt_addr_len);
        if(clnt_socket == -1)
            continue;
        else 
            cout << "\n[get connect]" << inet_ntoa(clnt_addr.sin_addr) << " port:" << ntohs(clnt_addr.sin_port)<<endl;

        pid = fork(); //此时，父子进程分别带有一个套接字
        if(pid == 0) // 子进程，向客户端提供回声服务
        {
            close(serv_socket); //关闭服务器套接字，因为从父进程传递到了子进程
            char buffer[1024];
            int len = 0 ;
            while((len = recv(clnt_socket,buffer,sizeof(buffer),0))> 0)
            {
                cout << "\n[get buffer] " << buffer << endl;
                send(clnt_socket,buffer,sizeof(len),0);
                write(fds[1],buffer,len);
            }
            close(clnt_socket);
            cout << "client shut down!" << endl;
            return 0;
        }
        else if(pid > 0) // 父进程
        {
            close(clnt_socket); //通过 accept 函数创建的套接字文件描述符已经复制给子进程，因为服务器端要销毁自己拥有的
        }
        
    }
    close(serv_socket);
    
    return 0 ;
}