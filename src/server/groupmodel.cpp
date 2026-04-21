#include "groupmodel.hpp"
#include "db.hpp"
#include <string.h>

/**
 * 创建群组
 */
bool GroupModel::createGroup(Group &group)
{
    char sql[1024];
    sprintf(sql, "insert into allgroup(groupname, groupdesc) values('%s', '%s')", group.getName().c_str(), group.getDesc().c_str());

    MySQL mysql;

    if (mysql.update(sql))
    {
        group.setId(mysql_insert_id(mysql.getConnection()));
        return true;
    }
    return false;
}
/**
 * 加入群组
 */
void GroupModel::addGroup(int userid, int groupid, string grouprole)
{
    char sql[1024];
    sprintf(sql, "insert into groupuser(groupid, userid, grouprole) values(%d, %d, '%s')", groupid, userid, grouprole.c_str());

    MySQL mysql;
    
    mysql.update(sql);
}
/**
 * 查询用户所在所有群组
 */
vector<Group> GroupModel::queryGroups(int userid)
{
    char sql[1024];
    sprintf(sql, "select b.id, b.groupname, b.groupdesc from groupuser a inner join allgroup b on a.groupid=b.id where a.userid=%d", userid);

    /**
     * 查询用户所在所有群组信息
     */
    vector<Group> vec;
    MySQL mysql;
    MYSQL_RES *res = mysql.query(sql);
    if (res != nullptr)
    {
        MYSQL_ROW row;
        while ((row = mysql_fetch_row(res)) != nullptr)
        {
            Group group;
            group.setId(atoi(row[0]));
            group.setName(row[1]);
            group.setDesc(row[2]);
            vec.push_back(group);
        }
        mysql_free_result(res);
    }

    /**
     * 查询群组详细成员信息
     */
    memset(sql, 0, strlen(sql));
    for (Group &group : vec)
    {
        sprintf(sql, "select b.id, b.name, b.state, a.grouprole from groupuser a inner join user b on a.userid = b.id where a.groupid = %d", group.getId());

        MYSQL_RES *res = mysql.query(sql);
        if (res != nullptr)
        {
            MYSQL_ROW row;
            while ((row = mysql_fetch_row(res)) != nullptr)
            {
                GroupUser user;
                user.setId(atoi(row[0]));
                user.setName(row[1]);
                user.setState(row[2]);
                user.setRole(row[3]);
                group.getUsers().push_back(user);
            }
            mysql_free_result(res);
        }
    }
    return vec;
}
/**
 * 查询群组中除用户外所有成员
 */
vector<GroupUser> GroupModel::queryGroupUsers(int userid, int groupid)
{
    char sql[1024];
    sprintf(sql, "select b.id, b.name, b.state, a.grouprole from groupuser a inner join user b on a.userid = b.id where a.groupid = %d and a.userid != %d", groupid, userid);

    vector<GroupUser> vec;
    MySQL mysql;
    
    MYSQL_RES* res = mysql.query(sql);
    if(res != nullptr) {
        MYSQL_ROW row;
        while ((row = mysql_fetch_row(res)) != nullptr) 
        {
            GroupUser user;
            user.setId(atoi(row[0]));
            user.setName(row[1]);
            user.setState(row[2]);
            user.setRole(row[3]);
            vec.push_back(user);
        }
        mysql_free_result(res);
    }
    return vec;
}