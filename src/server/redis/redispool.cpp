#include "redispool.hpp"
#include <iostream>
#include <string>
#include <cstring>

RedisPool &RedisPool::getInstance()
{
    static RedisPool redisPool;
    return redisPool;
}
RedisPool::~RedisPool()
{
    lock_guard<mutex> lock(poolMutex);

    while (!idleConnections.empty())
    {
        auto conn = idleConnections.front();
        idleConnections.pop();
        redisFree(conn);
    }
}
bool RedisPool::init(const string &host, const string &port, const string &passwd, size_t max_size)
{
    this->_host = host;
    this->_port = port;
    this->_passwd = passwd;
    this->max_size = max_size;
    lock_guard<mutex> lock(poolMutex);

    for (int i = 0; i < max_size; i++)
    {
        redisContext *redisConn = createConnection(host, port, passwd);
        if (redisConn == nullptr || redisConn->err)
        {
            if (redisConn)
            {
                std::cerr << "Connection error: " << redisConn->errstr << std::endl;
                redisFree(redisConn);
            }
            else
            {
                std::cerr << "Can't allocate redis context" << std::endl;
            }
            return false;
        }
        idleConnections.push(redisConn);
    }
    return true;
}
redisContext *RedisPool::createConnection(const string &host, const string &port, const string &passwd)
{
    // 连接Redis
    redisContext *c = redisConnect(host.c_str(), atoi(port.c_str()));
    if (c == nullptr || c->err)
    {
        if (c)
        {
            std::cerr << "Connection error: " << c->errstr << std::endl;
            redisFree(c);
        }
        else
        {
            std::cerr << "Can't allocate redis context" << std::endl;
        }
        return nullptr;
    }

    // 认证
    if (!passwd.empty())
    {
        redisReply *reply = (redisReply *)redisCommand(c, "AUTH %s", passwd.c_str());
        if (reply == nullptr || reply->type == REDIS_REPLY_ERROR)
        {
            std::cerr << "Redis auth failed" << std::endl;
            freeReplyObject(reply);
            redisFree(c);
            return nullptr;
        }
        freeReplyObject(reply);
    }

    return c;
}
redisContext *RedisPool::acquire()
{
    unique_lock<mutex> lock(poolMutex);

    while (idleConnections.empty() || active_count == max_size)
    {
        _cond.wait(lock);
    }

    auto conn = idleConnections.front();
    idleConnections.pop();
    active_count++;

    // 检测连接是否还有效
    redisReply *reply = (redisReply *)redisCommand(conn, "PING");
    if (reply && reply->type == REDIS_REPLY_STATUS && strcmp(reply->str, "PONG") == 0)
    {
        // 连接仍然有效，返回连接
        freeReplyObject(reply);
        return conn;
    }
    freeReplyObject(reply);

    // 连接无效创建新连接
    conn = createConnection(_host, _port, _passwd);
    if (conn == nullptr || conn->err)
    {
        if (conn)
        {
            std::cerr << "Connection error: " << conn->errstr << std::endl;
            redisFree(conn);
        }
        else
        {
            std::cerr << "Can't allocate redis context" << std::endl;
        }
        return nullptr;
    }
    return conn;
}
void RedisPool::release(redisContext *conn)
{
    lock_guard<mutex> lock(poolMutex);

    if (conn)
    {
        idleConnections.push(conn);
        active_count -= 1;
    }
    _cond.notify_one();
}