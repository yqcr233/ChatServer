#ifndef GROUPUSER_HPP
#define GROUPUSER_HPP

#include "user.hpp"
#include <string>
using namespace std;

/**
 * 群组用户，特殊用户的一种，多了群组权限信息
 */
class GroupUser : public User
{
public:
    void setRole(string role) { this->role = role; }
    string getRole() { return this->role; }

private:
    string role;
};

#endif
