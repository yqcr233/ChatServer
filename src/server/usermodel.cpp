#include "usermodel.hpp"
#include "db.h"
#include <iostream>
#include <vector>

// User表的增加方法
bool UserModel::insert(User &user)
{
    char sql[1024];
    sprintf(sql, "insert into user(name, password, state) values('%s', '%s', '%s')",
            user.getName().c_str(), user.getPwd().c_str(), user.getState().c_str());

    MySQL mysql;
    if (mysql.connect())
    {
        if (mysql.update(sql))
        {
            /**
             * 数据插入成功后，获取新用户数据逐渐id
             */
            user.setId(mysql_insert_id(mysql.getConnection()));
            return true;
        }
    }
    return false;
}

// 根据用户号码查询用户信息
User UserModel::query(string name)
{
    char sql[1024];
    sprintf(sql, "select * from user where name='%s' ", name.c_str());

    MySQL mysql;
    if (mysql.connect())
    {
        MYSQL_RES *res = mysql.query(sql);
        if (res != nullptr)
        {
            MYSQL_ROW row = mysql_fetch_row(res);
            if (row != nullptr)
            {
                User user;
                /**
                 * 返回值是一个string数组，表示每个属性值
                 */
                user.setId(atoi(row[0]));
                user.setName(row[1]);
                user.setPwd(row[2]);
                user.setState(row[3]);
                mysql_free_result(res);
                return user;
            }
        }
    }
    /**
     * 如果没找到返回一个默认空user，id=-1
     */
    return User();
}

User UserModel::query(int userid){
    char sql[1024];
    sprintf(sql, "select * from user where id=%d ", userid);

    MySQL mysql;
    if (mysql.connect())
    {
        MYSQL_RES *res = mysql.query(sql);
        if (res != nullptr)
        {
            MYSQL_ROW row = mysql_fetch_row(res);
            if (row != nullptr)
            {
                User user;
                /**
                 * 返回值是一个string数组，表示每个属性值
                 */
                user.setId(atoi(row[0]));
                user.setName(row[1]);
                user.setPwd(row[2]);
                user.setState(row[3]);
                mysql_free_result(res);
                return user;
            }
        }
    }
    /**
     * 如果没找到返回一个默认空user，id=-1
     */
    return User();
}

// 更新用户的状态信息
bool UserModel::updateState(User user)
{
    char sql[1024];
    sprintf(sql, "update user set state = '%s' where id = %d", user.getState().c_str(), user.getId());

    MySQL mysql;
    if (mysql.connect())
    {
        if (mysql.update(sql))
        {
            // 通知其他的在线好友用户状态改变
            return true;
        }
    }
    return false;
}

// 重置用户的状态信息
void UserModel::resetState()
{
    char sql[1024] = "update user set state = 'offline' where state = 'online'";

    MySQL mysql;
    if (mysql.connect())
    {
        mysql.update(sql);
    }
}

// 根据用户名前缀查询所有用户信息
vector<FriendUser> UserModel::queryUsersByPreName(int userid, string prename){
    char sql[1024];
    // userid, username, states, isfriend
    sprintf(sql, "SELECT \
                    u.id, \
                    u.name, \
                    u.state, \
                    IF(f.friendid IS NOT NULL, 1, 0) AS isFriend \
                    FROM user u \
                    LEFT JOIN friends f ON f.userid = %d AND f.friendid = u.id \
                    WHERE u.id != %d AND u.name LIKE CONCAT('%s', '%') \
                    ORDER BY u.name", userid, userid, prename.c_str());
    
    vector<FriendUser> users;
    MySQL mysql;
    if(mysql.connect()) {
        MYSQL_RES* res = mysql.query(sql);
        if(res != nullptr) {
            MYSQL_ROW row;
            while((row = mysql_fetch_row(res)) != nullptr) {
                FriendUser user;
                user.setId(atoi(row[0]));
                user.setName(row[1]);
                user.setState(row[2]);
                user.setIsFriend(atoi(row[3]));
                users.push_back(user);
            }
            mysql_free_result(res);
            return users;
        }
    }
    return users;
}