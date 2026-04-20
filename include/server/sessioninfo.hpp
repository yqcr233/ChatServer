#ifndef SESSIONINFO_HPP
#define SESSIONINFO_HPP
#include <string>
using namespace std;

class SessionInfo
{
public:
    SessionInfo(string chattype = "private", int sessionid = -1, string sessionname = "", string isonline = "offline", int64_t latesttime = -1, string lastmessage = "", int unreadcount = -1)
        : chatType(chattype), sessionId(sessionid), sessionName(sessionname), isOnline(isonline), latestTime(latesttime), lastMessage(lastmessage), unreadCount(unreadcount) {}

    void setChatType(string chattype) { chatType = chattype; }
    void setSessionId(int sessionid) { sessionId = sessionid; }
    void setSessionName(string sessionname) { sessionName = sessionname; }
    void setIsOnline(string isonline) { isOnline = isonline; }
    void setLatestTime(int64_t latesttime) { latestTime = latesttime; }
    void setLastMessage(string lastmessage) { lastMessage = lastmessage; }
    void setUnreadCount(int unreadcount) { unreadCount = unreadcount; }

    string getChatType() { return chatType; }
    int getSessionId() { return sessionId; }
    string getSessionName() { return sessionName; }
    string getIsOnline() { return isOnline; }
    int64_t getLatestTime() { return latestTime; }
    string getLastMessage() { return lastMessage; }
    int getUnreadCount() { return unreadCount; }

private:
    string chatType;    // 会话类型
    int sessionId;      // 会话对应的用户id或群聊id
    string sessionName; // 会话对应用户名或者群聊名
    string isOnline;    // 会话对象目前状态，群聊始终在线
    int64_t latestTime; // 最新消息发送时间
    string lastMessage; // 最新消息内容
    int unreadCount;    // 用户在该会话中的未读消息数
};

#endif