#include<iostream>
#include<sys/socket.h>
#include <arpa/inet.h>
#include<unistd.h>
using namespace std;

int main()
{
    int sock;
    struct sockaddr_in sock_addr;

    sock = socket(PF_INET,SOCK_STREAM,0);
    sock_addr.sin_family = AF_INET;
    sock_addr.sin_addr.s_addr = htonl(INADDR_ANY); //0.0.0.0 表示不确定地址，或所有地址、任意地址
    sock_addr.sin_port = htons(8888);
    
    connect(sock,(sockaddr *)&sock_addr,sizeof(sock_addr));

    while(1)
    {
        char send_buffer[1024];
        cout << "Input message(Q to quit) :";
        cin >> send_buffer;
        if(send_buffer[0] == 'Q') break;
        send(sock,send_buffer,sizeof(send_buffer),0);

        char recv_buffer[1024];
        recv(sock,recv_buffer,1024,0);
        cout << "[get buffer]" << recv_buffer << endl;
    }
    close(sock);
    
    return 0 ;
}