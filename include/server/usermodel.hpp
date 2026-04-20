#ifndef USER_MODEL_HPP
#define USER_MODEL_HPP

#include "user.hpp"
#include "frienduser.hpp"
#include <vector>

class UserModel{
public:
    // User表的增加方法
    bool insert(User &user);

    // 根据用户名查询用户信息
    User query(string name);

    // 根据用户名前缀查询所有用户信息
    vector<FriendUser> queryUsersByPreName(int userid, string prename);

    // 根据用户id查询用户信息
    User query(int userid);

    // 更新用户的状态信息
    bool updateState(User user);

    // 重置用户的状态信息
    void resetState();
};

#endif
