#include "chatserver.hpp"
#include <functional>
#include "json.hpp"
#include "chatservice.hpp"
#include "base64/base64.hpp"
#include "aesgcm/aesgcm.hpp"
#include <logger.hpp>

using namespace std;
using namespace placeholders;
using json = nlohmann::json;

ChatServer::ChatServer(EventLoop *loop,
                       const InetAddress &listenAddr,
                       const string &nameArg) : _server(loop, listenAddr, nameArg), _loop(loop)
{
    /**
     * 注册用户连接创建和断开回调
     */
    _server.setConnectionCallback(bind(&ChatServer::onConnection, this, _1));
    /**
     * 注册用户读写事件回调
     */
    _server.setMessageCallback(bind(&ChatServer::onMessage, this, _1, _2, _3));
    /**
     * 设置线程数大于1时，会默认分出一个线程作为新用户连接线程，其他为工作线程
     */
    _server.setThreadNum(4);
}

void ChatServer::start()
{
    _server.start();
}

void ChatServer::onConnection(const TcpConnectionPtr &con)
{
    if (!con->connected())
    {
        Chatservice::getInstance()->clientCloseException(con);    // 退出后改变客户端状态
        con->shutdown();
    }
}

void ChatServer::onMessage(const TcpConnectionPtr &con,
                           Buffer *buf,
                           TimeStamp time)
{
    string _buf = buf->retrieveAllAsString();
    Chatservice *service = Chatservice::getInstance();
    if(service->findAesOfConn(con)){ // 消息已经被aes+base64加密
        AesGcmManager aesMan = service->getAesOfConn(con);
        // 先解base64再解aes
        _buf = base64_decode(_buf);
        _buf = aesMan.decrypt(_buf);
    }
    fprintf(stdout, "%s\n", _buf.c_str());
    json js = json::parse(_buf);
    /**
     * 完全解耦网络模块代码和业务模块的代码
     * 在网络层server中没有调用任何业务层方法，而是通过映射表获取Handler
     */
    auto msgHandler = service->getMsgHandler(js["msgid"].get<int>());
    /**
     * 处理器执行相应业务处理
     */
    msgHandler(con, js, time);
}