#ifndef MYSQLPOOL_HPP
#define MYSQLPOOL_HPP

#include <string>
#include <mutex>
#include <mysql/mysql.h>
#include <queue>
#include <condition_variable>
#include <memory>

class MysqlPool {
public:
    /**
     * 获取数据库连接池唯一实例
     */
    static MysqlPool &instance() {
        static MysqlPool mysqlPool;
        return mysqlPool;
    }

    /**
     * 初始化连接池
     */
    bool init(const std::string& host = "127.0.0.1",
              const std::string& user = "root",
              const std::string& password = "12345",
              const std::string& db = "chat",
              int port = 3306,
              int poolSize = 5) {
        
        std::lock_guard<std::mutex> lock(_mutex);
        
        for (int i = 0; i < poolSize; ++i) {
            MYSQL* conn = createConnection(host, user, password, db, port);
            if (!conn) {
                return false;  // 任意一个连接失败就返回false
            }
            _connections.push(conn);
        }
        return true;
    }

    /**
     *  同步获取连接
     */
    MYSQL* getConnection() {
        std::unique_lock<std::mutex> lock(_mutex);
        
        // 等待直到有可用连接
        while (_connections.empty()) {
            _cond.wait(lock);
        }
        
        MYSQL* conn = _connections.front();
        _connections.pop();
        
        // 检查连接是否还活着
        if (mysql_ping(conn) != 0) {
            // 连接失效，创建新的
            conn = createConnection(_host, _usr, _password, _db, _port);
        }
        
        return conn;
    }

    /**
     * 归还连接
     */
    void releaseConnection(MYSQL* conn) {
        if (!conn) return;
        
        std::lock_guard<std::mutex> lock(_mutex);
        _connections.push(conn);
        _cond.notify_one();
    }

    /**
     * 获取当前空闲连接数
     */
    size_t idleCount() {
        std::lock_guard<std::mutex> lock(_mutex);
        return _connections.size();
    }

    ~MysqlPool() {
        std::lock_guard<std::mutex> lock(_mutex);
        while (!_connections.empty()) {
            MYSQL* conn = _connections.front();
            _connections.pop();
            mysql_close(conn);
        }
    }
private:
    MysqlPool() = default;
    
    MYSQL* createConnection(const std::string& host,
                           const std::string& user,
                           const std::string& password,
                           const std::string& db,
                           int port) {
        MYSQL* conn = mysql_init(nullptr);
        if (!conn) return nullptr;
        
        mysql_options(conn, MYSQL_SET_CHARSET_NAME, "utf8mb4");
        
        if (!mysql_real_connect(conn, host.c_str(), user.c_str(),
                                password.c_str(), db.c_str(), port, nullptr, 0)) {
            mysql_close(conn);
            return nullptr;
        }
        
        mysql_set_character_set(conn, "utf8mb4");
        mysql_query(conn, "SET NAMES utf8mb4");
        return conn;
    }

    std::queue<MYSQL*> _connections;
    std::mutex _mutex;
    std::condition_variable _cond;

    std::string _host, _usr, _password,_db;
    int _port = 3306;
};

#endif