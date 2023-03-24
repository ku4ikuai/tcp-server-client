# tcp-server-client
参考《TCP/IP网络编程》(韩-尹圣雨)学习笔记，将里面的参考代码重新用c++复现了一下。
https://github.com/riba2534/TCP-IP-NetworkNote

### 1、ch4 echo-server-client

​	编写一个回声（echo）服务器/客户端。顾名思义，服务端将客户端传输的字符串数据原封不动的传回客户端，就像回声一样。

​	程序运行的基本方式：

- 服务器端在**同一时刻只与一个客户端相连**，并提供回声服务。
- 服务器端依次向 5 个客户端提供服务并退出。
- 客户端接受用户输入的字符串并发送到服务器端。
- 服务器端将接受的字符串数据传回客户端，即「回声」
- 服务器端与客户端之间的字符串回声一直执行到客户端输入 Q 为止。

### 2、ch10 echo-mp 多进程

​	基于多进程实现的并发的回声服务器的服务端。

### 3、ch12 echo-select select io复用

缺点：第 12 章实现了基于 select 的 I/O 复用技术服务端，其中有不合理的设计如下：

- 调用 select 函数后常见的**针对所有文件描述符的循环语句**
- 每次调用 select 函数时都需要向该函数传递监视对象信息

上述两点可以从 [echo_selectserv.c](https://github.com/riba2534/TCP-IP-NetworkNote/blob/master/ch12/echo_selectserv.c) 得到确认，**调用 select 函数后，并不是把发生变化的文件描述符单独集中在一起，而是通过作为监视对象的 fd_set 变量的变化，找出发生变化的文件描述符，因此无法避免针对所有监视对象的循环语句。**

而且，作为监视对象的 fd_set 会发生变化，所以调用 select 函数前应该复制并保存原有信息，**并在每次调用 select 函数时传递新的监视对象信息**

优点：select 的兼容性比较高，这样就可以支持很多的操作系统，不受平台的限制，使用 select 函数满足以下两个条件：

- 服务器接入者少
- 程序应该具有兼容性

### 4、ch17 echo-epoll  epoll io复用

​	优点：

- 无需编写以监视状态变化为目的的针对所有文件描述符的循环语句
- 调用对应于 select 函数的 epoll_wait 函数时无需每次传递监视对象信息。

  

