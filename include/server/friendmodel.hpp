#ifndef FRIENDMODEL_HPP
#define FRIENDMODEL_HPP
#include "user.hpp"
#include <vector>
using namespace std;

class FriendModel
{
public:
    /**
     * 添加好友关系
     */
    void insert(int userid, int friendid);
    /**
     * 解除好友关系
     */
    void delete_(int userid, int friendid);
    /**
     * 查询所有好友
     */
    vector<User> query(int userid);
    /**
     * 根据前缀查询好友
     */
    vector<User> query(int userid, string prename);
    /**
     * 查询当前在线好友
     */
    vector<int> queryOnlineFriends(int userid);
};

#endif