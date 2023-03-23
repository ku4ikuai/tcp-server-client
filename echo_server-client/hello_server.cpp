#include<iostream>
#include<sys/socket.h>
#include <arpa/inet.h>
#include<unistd.h>
using namespace std;

int main()
{
    int serv_socket;
    int clnt_socket;
    struct sockaddr_in serv_addr;
    struct sockaddr_in clnt_addr;
    socklen_t clnt_addr_len = sizeof(clnt_addr);

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

        char buffer[1024];
        int len;
        while((len = recv(clnt_socket,buffer,sizeof(buffer),0)) != 0)
        {
            cout << "\n[get buffer] " << buffer << endl;
            send(clnt_socket,buffer,len,0);
        }
        
        close(clnt_socket);
        cnt++;
    }
    close(serv_socket);
    
    return 0 ;
}