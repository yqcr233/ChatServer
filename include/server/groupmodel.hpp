#ifndef GROUPMODEL_HPP
#define GROUPMODEL_HPP

#include "group.hpp"
#include <string>
#include <vector>
using namespace std;

class GroupModel
{
public:
    /**
     * 创建群组
     */
    bool createGroup(Group &group);
    /**
     * 加入群组
     */
    void addGroup(int userid, int groupid, string grouprole);
    /**
     * 查询用户所在所有群组
     */
    vector<Group> queryGroups(int userid);
    /**
     * 查询群组中除用户外所有成员
     */
    vector<GroupUser> queryGroupUsers(int userid, int groupid);
};

#endif