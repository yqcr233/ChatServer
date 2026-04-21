#include "friendmodel.hpp"
#include "db.h"

/**
 * 添加好友关系
 */
void FriendModel::insert(int userid, int friendid)
{
    char sql[1024];
    sprintf(sql, "insert into friends(userid, friendid) values(%d, %d)", userid, friendid);

    MySQL mysql;
    if (mysql.connect())
    {
        mysql.update(sql);
    }
}
/**
 * 查询所有好友
 */
vector<User> FriendModel::query(int userid)
{
    char sql[1024];
    sprintf(sql, "select a.id, a.name, a.state from user a inner join friends b on a.id = b.friendid where b.userid = %d", userid);

    vector<User> vec;
    MySQL mysql;
    if (mysql.connect())
    {
        MYSQL_RES *res = mysql.query(sql);
        if (res != nullptr)
        {
            MYSQL_ROW row;
            while ((row = mysql_fetch_row(res)) != nullptr)
            {
                User user;
                user.setId(atoi(row[0]));
                user.setName(row[1]);
                user.setState(row[2]);
                vec.push_back(user);
            }
            mysql_free_result(res);
            return vec;
        }
    }

    return vec;
}
vector<User> FriendModel::query(int userid, string prename)
{
    char sql[1024];
    sprintf(sql, "select a.id, a.name, a.state from user a inner join friends b on a.id = b.friendid where b.userid = %d and a.name like concat('%s','%') ", userid, prename.c_str());

    vector<User> users;
    MySQL mysql;
    if (mysql.connect())
    {
        MYSQL_RES *res = mysql.query(sql);
        if (res != nullptr)
        {
            MYSQL_ROW row;
            while ((row = mysql_fetch_row(res)) != nullptr)
            {
                User user;
                user.setId(atoi(row[0]));
                user.setName(row[1]);
                user.setState(row[2]);
                users.push_back(user);
            }
            mysql_free_result(res);
            return users;
        }
    }
    return users;
}
/**
 * 查询当前在线好友
 */
vector<int> FriendModel::queryOnlineFriends(int userid)
{
    char sql[1024];
    sprintf(sql, "select f.friendid from friends f join user u on u.id = f.friendid where f.userid = %d and u.state='online'", userid);

    vector<int> friends;
    MySQL mysql;
    if (mysql.connect())
    {
        MYSQL_RES *res = mysql.query(sql);
        if (res != nullptr)
        {
            MYSQL_ROW row;
            while ((row = mysql_fetch_row(res)) != nullptr)
            {
                friends.push_back(atoi(row[0]));
            }
            mysql_free_result(res);
            return friends;
        }
    }
    return friends;
}

/**
 * 解除好友关系
 */
void FriendModel::delete_(int userid, int friendid)
{
    char sql[1024];
    sprintf(sql, "delete from friends where (userid=%d and friendid=%d) or (userid=%d and friendid=%d)", userid, friendid, friendid, userid);

    MySQL mysql;
    if (mysql.connect())
    {
        mysql.update(sql);
    }
}