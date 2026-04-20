#ifndef MESSAGEMODEL_HPP
#define MESSAGEMODEL_HPP

#include <string>
#include <vector>
#include "message.hpp"
#include "sessioninfo.hpp"

using namespace std;

class MessageModel
{
public:
    /**
     * 插入一条消息记录
     */
    void insert(int toid = -1, int fromid = -1, int groupid = -1, string msg = "", int state = 0, int64_t createAt = -1);

    /**
     * 查询用户所有会话消息便于初始化
     */
    vector<SessionInfo> querySessions(int userid);

    /**
     * 查询指定单聊会话所有消息
     */
    vector<Message> queryOne(int userid, int sessionid);

    /**
     * 查询指定群聊会话所有消息
     */
    vector<Message> queryGroup( int sessionid);
};

#endif