#ifndef FRIENDUSER_HPP
#define FRIENDUSER_HPP
#include "user.hpp"

class FriendUser : public User
{
public:
    FriendUser(int id = -1, string name = "", string pwd = "", string state = "offline", int isfriend = 0) : User(id, name, pwd, state)
    {
        this->isfriend = isfriend;
    }

    void setIsFriend(int isfriend) {this->isfriend = isfriend;}
    int getIsFriend() {return isfriend;}

private:
    int isfriend;
};

#endif