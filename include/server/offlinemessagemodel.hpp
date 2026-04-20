#ifndef OFFLINEMESSAGEMODEL_HPP
#define OFFLINEMESSAGEMODEL_HPP
#include <string>
#include <vector>
using namespace std;

class OfflineMessageModel
{
public:
    /**
     * 插入一条离线消息
     */
    void insert(int userid, string msg);
    /**
     * 移除用户的离线消息
     */
    void remove(int userid);
    /**
     * 查询用户的离线消息
     */
    vector<string> query(int userid);
};

#endif