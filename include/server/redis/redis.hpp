#ifndef REDIS_HPP
#define REDIS_HPP

#include <string>
#include <hiredis/hiredis.h>
#include <functional>
#include "redispool.hpp"

using namespace std;

class Redis
{
public:
    using NotifyHandler = function<void(int, string)>;

    Redis();
    ~Redis();

    /**
     * 向redis指定通道发送消息
     */
    bool publish(int channel, string msg);

    /**
     * 向redis指定通道订阅和取消订阅消息
     */
    bool subcribe(int channel);
    bool unSubcribe(int channel);

    /**
     * 在独立线程中接收通道中的消息
     */
    void observerChannelMessage();

    /**
     * 初始化向业务层上报通道消息的回调对象
     */
    void initNotifyHandler(NotifyHandler fn);

private:
    // publish的redis通道，复用连接池连接
    redisContext *publishContext;

    // subcribe的redis通道，随用随创建
    redisContext *subcribeContext;

    NotifyHandler notifyMsgHandler;
};
#endif