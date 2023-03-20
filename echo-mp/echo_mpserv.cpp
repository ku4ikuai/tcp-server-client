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
    int cnt = 0;
    while(cnt != 5)
    {
        clnt_socket = accept(serv_socket,(sockaddr *)&clnt_addr,&clnt_addr_len);
        if(clnt_socket != 0) cout << "\n[get connect]" << inet_ntoa(clnt_addr.sin_addr) << " port:" << ntohs(clnt_addr.sin_port)<<endl;

        pid_t pid = fork();
        if(pid == 0) // 子进程
        {
            close(serv_socket);
            char buffer[1024];
            while(recv(clnt_socket,buffer,sizeof(buffer),0) > 0)
            {
                cout << "\n[get buffer] " << buffer << endl;
                send(clnt_socket,buffer,sizeof(buffer),0);
            }
            close(clnt_socket);
            return 0;
        }
        else if(pid > 0) // 父进程
        {
            close(clnt_socket);
        }
        
        cnt++;
    }
    close(serv_socket);
    
    return 0 ;
}