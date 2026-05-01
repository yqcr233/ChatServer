#ifndef REDISPOOL_HPP
#define REDISPOOL_HPP

#include <hiredis/hiredis.h>
#include <memory>
#include <queue>
#include <condition_variable>
#include <mutex>
#include <functional>

using namespace std;

class RedisPool
{
public:
    static RedisPool &getInstance();

    // 禁止拷贝
    RedisPool(const RedisPool &) = delete;
    RedisPool &operator=(const RedisPool &) = delete;

    bool init(const string &host = "127.0.0.1", const string &port = "6379", const string &passwd = "12345", size_t max_size = 20);

    redisContext *acquire();

    void release(redisContext *conn);

    ~RedisPool();

private:
    RedisPool() = default;

    redisContext *createConnection(const string &host, const string &port, const string &passwd);

    queue<redisContext *> idleConnections;
    mutex poolMutex;
    condition_variable _cond;
    string _host, _port, _passwd;
    size_t max_size;
    size_t active_count = 0;
};

#endif