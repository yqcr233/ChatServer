#include "db.hpp"
#include "logger.hpp"

// 数据库配置信息
static string server = "127.0.0.1";
static string user = "root";
static string password = "12345";
static string dbname = "chat";

MySQL::MySQL()
{
    // _conn = mysql_init(nullptr);
    // // 连接前设置选项
    // mysql_options(_conn, MYSQL_SET_CHARSET_NAME, "utf8mb4");

    _conn = MysqlPool::instance().getConnection();
}
// 释放数据库连接资源
MySQL::~MySQL()
{
    if (_conn != nullptr)
        MysqlPool::instance().releaseConnection(_conn);
}
// 更新操作
bool MySQL::update(string sql)
{
    if (mysql_query(_conn, sql.c_str()))
    {
        LOG_INFO("%s:%d:%s更新失败！\n", __FILE__, __LINE__, sql.c_str());
        return false;
    }
    return true;
}
// 查询操作
MYSQL_RES *MySQL::query(string sql)
{
    if (mysql_query(_conn, sql.c_str()))
    {
        LOG_INFO("%s:%d:%s查询失败！\n", __FILE__, __LINE__, sql.c_str());
        return nullptr;
    }
    return mysql_use_result(_conn);
}
// 获取连接
MYSQL *MySQL::getConnection()
{
    return _conn;
}