#ifndef REQUESTMODEL_HPP
#define REQUESTMODEL_HPP

#include "friendrequest.hpp"
#include <vector>

class RequestModel
{
public:
    /**
     * 插入一条好友请求
     */
    void insert(FriendRequest req);

    /**
     * 查询指定用户的所有好友请求
     */
    vector<FriendRequest> query(int userid);    

    /**
     * 同意好友请求时，移除所有该用户的好友请求
     */
    void removeAccept(int userid, int fromid);

    /**
     * 拒绝好友请求时，只移除该条好友请求
     */
    void removeRefuse(int requestid);
};

#endif