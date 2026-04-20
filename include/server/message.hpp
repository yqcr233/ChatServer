#ifndef MESSAGE_HPP
#define MESSAGE_HPP

#include <string>
using namespace std;

class Message
{
public:
    Message(int toid = -1, int fromid = -1, int groupid = -1, string msg = "", int state = 0, int64_t createAt = -1, string fromname = "")
    {
        this->toId = toid;
        this->fromId = fromid;
        this->msg = msg;
        this->state = state;
        this->createAt = createAt;
        this->groupId = groupid;
        this->fromName = fromname;
    }

    void setToId(int toid) { this->toId = toid; }
    void setFromId(int fromId) { this->fromId = fromId; }
    void setGroupId(int groupId) { this->groupId = groupId; }
    void setMsg(string msg) { this->msg = msg; }
    void setState(int state) { this->state = state; }
    void setCreateAt(int64_t createAt) { this->createAt = createAt; }
    void setFromName(string fromname) { this->fromName = fromname; }

    int getToId() const { return toId; }
    int getFromId() const { return fromId; }
    int getGroupId() const { return groupId; }
    string getMsg() const { return msg; }
    int getState() const { return state; }
    int64_t getCreateAt() const { return createAt; }
    string getFromName() const { return fromName; }

private:
    int toId;
    int fromId;
    int groupId;
    string msg;
    int state;
    int64_t createAt;
    string fromName;
};

#endif