#include "chatserver.hpp"
#include "chatservice.hpp"
#include "mysqlpool.hpp"
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

    if (!MysqlPool::instance().init("127.0.0.1", "root", "12345", "chat", 3306, 5))
    {
        std::cerr << "数据库连接池初始化失败！" << std::endl;
        return 1;
    }
    
    EventLoop loop; 
    // InetAddress addr("192.168.152.100", 9999);
    InetAddress addr(9999, "192.168.152.100");
    ChatServer server(&loop, addr, "EchoServer");
    
    RsaKeyManager rsaManager;

    server.start();
    loop.loop();           // 等于epoll_wait，以阻塞的方式等待新用户连接，和已连接用户的读写事件等
    return 0;
}
