#include "redis/redis.hpp"
#include <cstring>
#include <iostream>
#include <thread>

Redis::Redis()
{
    /**
     * 获取发布-订阅消息redis连接
     */
    publishContext = RedisPool::getInstance().acquire();
    subcribeContext = RedisPool::getInstance().acquire();

    /**
     * 在单独线程中监听通道订阅消息
     */
    thread t([&] {
        observerChannelMessage();
    });
    t.detach();
}
Redis::~Redis()
{
    if (publishContext)
    {
        RedisPool::getInstance().release(publishContext);
    }
    if (subcribeContext)
    {
        RedisPool::getInstance().release(subcribeContext);
    }
}
bool Redis::publish(int channel, string msg)
{
    redisReply *reply = (redisReply *)redisCommand(publishContext, "PUBLISH %d %s", channel, msg.c_str());
    if (reply == nullptr)
    {
        cerr << "publish command failed!" << endl;
        return false;
    }
    freeReplyObject(reply);
    return true;
}
bool Redis::subcribe(int channel)
{
    /**
     * 只订阅不接收消息，消息通过后台线程接收
     */
    if (redisAppendCommand(this->subcribeContext, "SUBSCRIBE %d", channel) == REDIS_ERR)
    {
        cerr << "subscribe command failed!" << endl;
        return false;
    }

    int done = 0;
    while (!done)
    {
        if (redisBufferWrite(this->subcribeContext, &done) == REDIS_ERR)
        {
            cerr << "subscribe command failed!" << endl;
            return false;
        }
    }

    // 在后台线程中接收响应
    return true;
}
bool Redis::unSubcribe(int channel)
{
    if (redisAppendCommand(this->subcribeContext, "UNSUBSCRIBE %d", channel) == REDIS_ERR)
    {
        cerr << "subscribe command failed!" << endl;
        return false;
    }

    int done = 0;
    while (!done)
    {
        if (redisBufferWrite(this->subcribeContext, &done) == REDIS_ERR)
        {
            cerr << "subscribe command failed!" << endl;
            return false;
        }
    }
    return true;
}
void Redis::observerChannelMessage()
{
    redisReply *reply = nullptr;
    // redisGetReply会阻塞所在线程，直到接收响应
    while (redisGetReply(this->subcribeContext, (void **)&reply) == REDIS_OK)
    {
        if (reply != nullptr && reply->element[1] != nullptr && reply->element[2]->str != nullptr)
        {
            string msgType(reply->element[0]->str, reply->element[0]->len);
            if (msgType == "message")
            {
                // 上传应用层处理实际响应数据
                notifyMsgHandler(atoi(reply->element[1]->str), reply->element[2]->str);
            }
            else
            {
                // 打印日志消息
                fprintf(stdout, "redisLog: %s %d\n", msgType.c_str(), atoi(reply->element[1]->str));
            }
        }
    }
}
void Redis::initNotifyHandler(NotifyHandler fn)
{
    this->notifyMsgHandler = fn;
}