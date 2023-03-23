#include<iostream>
#include<sys/socket.h>
#include <arpa/inet.h>
#include<unistd.h>
using namespace std;

const int BUF_SIZE = 1024;

int main()
{
    int sock;
    struct sockaddr_in sock_addr;

    sock = socket(PF_INET,SOCK_STREAM,0);
    if(sock == -1) 
    {
        cout << "socket error" << endl;
        exit(1);
    }
    memset(&sock_addr,0,sizeof(sock_addr));
    sock_addr.sin_family = AF_INET;
    sock_addr.sin_addr.s_addr = htonl(INADDR_ANY); //0.0.0.0 表示不确定地址，或所有地址、任意地址
    sock_addr.sin_port = htons(8888);
    
    if(connect(sock,(sockaddr *)&sock_addr,sizeof(sock_addr)) == -1)
    {
        cout << "connect error" << endl;
        exit(1);
    }
    else 
        cout << "connected...." << endl;

    while(1)
    {
        char send_buffer[1024];
        cout << "Input message(Q to quit) :";
        cin >> send_buffer;
        if(send_buffer[0] == 'Q')
        {
            shutdown(sock,SHUT_WR);  // 断开输出流，断开输出流时向主机传输EOF
            cout << "shut down!\n";
            return 0;
        }
        cout << strlen(send_buffer) << endl;
        send(sock,send_buffer,strlen(send_buffer)+1,0);

        char recv_buffer[1024];
        recv(sock,recv_buffer,BUF_SIZE-1,0);
        cout << "[get buffer]" << recv_buffer << endl;
    }
    close(sock);
    
    return 0 ;
}