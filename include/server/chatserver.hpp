#ifndef CHATSERVER_HPP
#define CHATSERVER_HPP


#include "tcp_server.hpp"
#include "event_loop.hpp"
#include "callbacks.hpp"
#include <iostream>
#include <string>

using namespace std;

class ChatServer
{
public:
    // 初始化TcpServer
    ChatServer(EventLoop *loop,
               const InetAddress &listenAddr,
               const string &nameArg);
    // 启动ChatServer服务
    void start();

private:
    // TcpServer绑定的回调函数，当有新连接或连接中断时调用
    void onConnection(const TcpConnectionPtr &con);
    // TcpServer绑定的回调函数，当有新数据时调用
    void onMessage(const TcpConnectionPtr &con,
                   Buffer *buf,
                   TimeStamp time);

private:
    TcpServer _server;
    EventLoop *_loop;
    string privateRsa;
};

#endif