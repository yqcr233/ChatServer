#include "chatserver.hpp"
#include "chatservice.hpp"
#include <iostream>
#include <signal.h>
#include "rsa.hpp"
using namespace std;

void resetHandler(int) {
    Chatservice::getInstance()->reset();
    exit(0);
}

int main(int argc, char const *argv[])
{
    /**
     *  服务器ctrl+c中断信号时触发客户端状态重置函数
     */
    signal(SIGINT, resetHandler);

    EventLoop loop;
    InetAddress addr("192.168.152.100", 9999);
    ChatServer server(&loop, addr, "EchoServer");
    
    RsaKeyManager rsaManager;

    server.start();
    loop.loop();           // 等于epoll_wait，以阻塞的方式等待新用户连接，和已连接用户的读写事件等
    return 0;
}
