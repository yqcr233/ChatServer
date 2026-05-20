#include "chatserver.hpp"
#include "chatservice.hpp"
#include "mysqlpool.hpp"
#include "redispool.hpp"
#include <iostream>
#include <signal.h>
#include <fstream>
#include "json.hpp"
#include "rsa.hpp"
using namespace std;

void resetHandler(int) {
    Chatservice::getInstance()->reset();
    exit(0);
}

int main(int argc, char **argv)
{
    /**
     *  服务器ctrl+c中断信号时触发客户端状态重置函数
     */
    signal(SIGINT, resetHandler);

    if(argc < 3){
        fprintf(stderr, "Usage: %s ip port [config_file]\n", argv[0]);
        return 1;
    }
    char* ip = argv[1];
    uint16_t port = atoi(argv[2]);
    string configPath = argc >= 4 ? argv[3] : "config.json";

    // 读取配置文件
    json config;
    try
    {
        ifstream configFile(configPath);
        if (!configFile.is_open())
        {
            std::cerr << "错误：无法打开配置文件 " << configPath << std::endl;
            return 1;
        }
        config = json::parse(configFile);
    }
    catch (const json::exception &e)
    {
        std::cerr << "错误：配置文件解析失败 - " << e.what() << std::endl;
        return 1;
    }

    // 读取 MySQL 配置
    string db_host = config.value("mysql", json::object()).value("host", "127.0.0.1");
    int db_port = config.value("mysql", json::object()).value("port", 3306);
    string db_user = config.value("mysql", json::object()).value("user", "");
    string db_pass = config.value("mysql", json::object()).value("password", "");
    string db_name = config.value("mysql", json::object()).value("database", "chat");
    int db_pool_size = config.value("mysql", json::object()).value("pool_size", 5);

    if (db_pass.empty())
    {
        std::cerr << "错误：请在配置文件 " << configPath << " 中设置 mysql.password" << std::endl;
        return 1;
    }

    if (!MysqlPool::instance().init(db_host, db_user, db_pass, db_name, db_port, db_pool_size))
    {
        std::cerr << "数据库连接池初始化失败！" << std::endl;
        return 1;
    }

    // 读取 Redis 配置
    string redis_host = config.value("redis", json::object()).value("host", "127.0.0.1");
    int redis_port = config.value("redis", json::object()).value("port", 6379);
    string redis_pass = config.value("redis", json::object()).value("password", "");
    int redis_pool_size = config.value("redis", json::object()).value("pool_size", 20);

    if (!RedisPool::getInstance().init(redis_host, to_string(redis_port), redis_pass, redis_pool_size))
    {
        std::cerr << "Redis连接池初始化失败！" << std::endl;
        return 1;
    }
    
    EventLoop loop;
    InetAddress addr(port, ip);
    ChatServer server(&loop, addr, "EchoServer");
    
    RsaKeyManager rsaManager;

    server.start();
    loop.loop();           // 等于epoll_wait，以阻塞的方式等待新用户连接，和已连接用户的读写事件等
    return 0;
}
