#include "chatservice.hpp"
#include "public.hpp"
// #include <muduo/base/Logging.h>
#include <logger.hpp>
#include "base64/base64.hpp"

using namespace std::placeholders;

Chatservice *Chatservice::getInstance()
{
    static Chatservice service;
    return &service;
}

Chatservice::Chatservice()
{
    /**
     * 这里核心，将网络层和业务层进行解耦
     * 网络层和业务层通过一个抽象的消息处理映射表进行交互，网络层只关心消息的接收和发送，而业务层只关心具体的业务逻辑
     */
    _msgHandlerMap.insert({LOGIN_MSG, std::bind(&Chatservice::login, this, _1, _2, _3)});
    _msgHandlerMap.insert({REG_MSG, std::bind(&Chatservice::reg, this, _1, _2, _3)});
    _msgHandlerMap.insert({ADD_FRIEND_MSG, std::bind(&Chatservice::addFriend, this, _1, _2, _3)});
    _msgHandlerMap.insert({ONE_CHAT_MSG, std::bind(&Chatservice::oneChat, this, _1, _2, _3)});
    _msgHandlerMap.insert({LOGINOUT_MSG, std::bind(&Chatservice::loginOut, this, _1, _2, _3)});
    // _msgHandlerMap.insert({CREATE_GROUP_MSG, std::bind(&Chatservice::createGroup, this, _1, _2, _3)});
    _msgHandlerMap.insert({ADD_GROUP_MSG, std::bind(&Chatservice::addGroup, this, _1, _2, _3)});
    _msgHandlerMap.insert({GROUP_CHAT_MSG, std::bind(&Chatservice::groupChat, this, _1, _2, _3)});
    _msgHandlerMap.insert({HEART_MSG, std::bind(&Chatservice::heartCheck, this, _1, _2, _3)});
    _msgHandlerMap.insert({GET_SESSION_MESSAGE, std::bind(&Chatservice::getSessionHistory, this, _1, _2, _3)});
    _msgHandlerMap.insert({FIND_FRIENDS, std::bind(&Chatservice::getFriends, this, _1, _2, _3)});
    _msgHandlerMap.insert({SEARCH_USERS, std::bind(&Chatservice::getUsers, this, _1, _2, _3)});
    _msgHandlerMap.insert({FRIEND_REQUEST, std::bind(&Chatservice::friendRequest, this, _1, _2, _3)});
    _msgHandlerMap.insert({GET_FRIEND_REQUEST, std::bind(&Chatservice::friendRequestReturn, this, _1, _2, _3)});
    _msgHandlerMap.insert({REMOVE_REQUEST_ACCEPT, std::bind(&Chatservice::acceptFriendRequest, this, _1, _2, _3)});
    _msgHandlerMap.insert({REMOVE_REQUEST_REFUSE, std::bind(&Chatservice::rejectFriendRequest, this, _1, _2, _3)});
    _msgHandlerMap.insert({CREATE_GROUP_MSG, std::bind(&Chatservice::createGroupChat, this, _1, _2, _3)});
    _msgHandlerMap.insert({SETRSAAESKEY, std::bind(&Chatservice::setRsaAesKey, this, _1, _2, _3)});
}

MsgHandler Chatservice::getMsgHandler(int msgid)
{
    auto it = _msgHandlerMap.find(msgid);
    if (it == _msgHandlerMap.end())
    {
        // 没找到对应消息处理器返回一个空操作
        return [=](const TcpConnectionPtr &conn, json &js, TimeStamp time)
        {
            // LOG_ERROR << "msgid:" << msgid << " can not find handler!";
            LOG_ERROR("msgid: %d can not find handler!\n", msgid);
        };
    }
    else
    {
        return _msgHandlerMap[msgid];
    }
}

void Chatservice::login(const TcpConnectionPtr &conn, json &js, TimeStamp time)
{
    /**
     * 登录协议为：msgid, name, pwd
     * 消息回复协议为：msgid，errno，...(成功返回相关信息，失败[errno!=0]返回错误消息)
     */
    // int id = js["id"].get<int>();
    string name = js["name"];
    string pwd = js["pwd"];

    User user = _userModel.query(name);
    if (user.getName() == name && user.getPwd() == pwd)
    {
        int id = user.getId();
        if (user.getState() == "online")
        {
            /**
             * 用户已登录不能重复登录
             */
            json res;
            res["msgid"] = LOGIN_MSG_ACK;
            res["errno"] = 1;
            res["errmsg"] = "this account is using, input another!";
            // conn->send(res.dump());
            sendMsg(conn, res.dump(), time);
        }
        else
        {
            {
                lock_guard<mutex> lock(_connMutex);
                _userConnMap.insert({id, conn});
            }

            // 登录成功，更新用户状态,并通知在线好友
            user.setState("online");
            _userModel.updateState(user);
            vector<int> friendOnline = _friendModel.queryOnlineFriends(user.getId());
            for (int &f : friendOnline)
            {
                /**
                 * 好友状态恢复协议：msgid，friendid
                 */
                json notice;
                notice["msgid"] = NOTICE_FRIENDSTATE;
                notice["friendid"] = user.getId();
                auto it = _userConnMap.find(f);
                if (it != _userConnMap.end())
                {
                    // LOG_INFO << notice.dump() << " " << f;
                    LOG_INFO("%s\n", notice.dump().c_str());
                    // it->second->send(notice.dump() + "\n");
                    sendMsg(it->second, notice.dump(), time);
                }
            }

            json res;
            res["msgid"] = LOGIN_MSG_ACK;
            res["errno"] = 0;
            res["userid"] = user.getId();
            res["username"] = user.getName();

            /**
             * 查询好友列表并返回
             */
            vector<User> friendVec = _friendModel.query(id);
            if (!friendVec.empty())
            {
                vector<string> vec2;
                for (User &user : friendVec)
                {
                    json js;
                    js["id"] = user.getId();
                    js["name"] = user.getName();
                    js["state"] = user.getState();
                    vec2.push_back(js.dump());
                }
                res["friends"] = vec2;
            }
            else
            {
                res["friends"] = "";
            }

            /**
             * 查询会话列表并返回
             */
            vector<SessionInfo> sessionVec = _messageModel.querySessions(id);
            if (!sessionVec.empty())
            {
                vector<string> vec;
                for (SessionInfo &info : sessionVec)
                {
                    json js;
                    js["chattype"] = info.getChatType(); // private or group
                    js["sessionid"] = info.getSessionId();
                    js["sessionname"] = info.getSessionName();
                    js["isonline"] = info.getIsOnline();
                    js["latesttime"] = info.getLatestTime();
                    js["lastmessage"] = info.getLastMessage();
                    js["unreadcount"] = info.getUnreadCount();
                    vec.push_back(js.dump());
                }
                res["sessions"] = vec;
            }
            else
            {
                res["sessions"] = "";
            }

            // conn->send(res.dump() + "\n");
            sendMsg(conn, res.dump(), time);
        }
    }
    else
    {
        json res;
        res["msgid"] = LOGIN_MSG_ACK;
        res["errno"] = 2;
        res["errmsg"] = "id or password is invalid!";
        // conn->send(res.dump() + "\n");
        sendMsg(conn, res.dump(), time);
    }
}

void Chatservice::loginOut(const TcpConnectionPtr &conn, json &js, TimeStamp time)
{
    /**
     * msgid, id
     */
    int userid = js["id"].get<int>();
    {
        lock_guard<mutex> lock(_connMutex);
        auto it = _userConnMap.find(userid);
        if (it != _userConnMap.end())
        {
            _userConnMap.erase(it);
        }
    }

    User user(userid, "", "", "offline");
    _userModel.updateState(user);
}

void Chatservice::reg(const TcpConnectionPtr &conn, json &js, TimeStamp time)
{
    /**
     * 注册用户协议为：msgid, name, pwd
     * 回复协议为：msgid，errno，errmsg(可选)
     */

    string name = js["name"];
    string pwd = js["pwd"];

    User user;
    user.setName(name);
    user.setPwd(pwd);
    bool state = _userModel.insert(user);
    if (state)
    {
        json res;
        res["msgid"] = REG_MSG_ACK;
        res["errno"] = 0;
        res["id"] = user.getId();
        // conn->send(res.dump() + "\n");
        sendMsg(conn, res.dump(), time);
    }
    else
    {
        json res;
        res["msgid"] = REG_MSG_ACK;
        res["errno"] = 1;
        res["errmsg"] = "register fail!";
        // conn->send(res.dump() + "\n");
        sendMsg(conn, res.dump(), time);
    }
}

void Chatservice::oneChat(const TcpConnectionPtr &conn, json &js, TimeStamp time)
{
    /**
     * msgid, fromid, frommsg, toid, createat, sessiontype
     * 转发内容一致
     */
    int toid = js["toid"].get<int>();
    int fromid = js["fromid"].get<int>();
    string frommsg = js["frommsg"];
    int64_t createat = js["createat"].get<int64_t>();
    User user = _userModel.query(toid);

    // 插入历史消息
    _messageModel.insert(toid, fromid, -1, frommsg, 1, createat);

    // LOG_INFO << toid << " " << fromid << " " << user.getState();
    LOG_INFO("toid:%d fromid:%d userState:%s\n", toid, fromid, user.getState().c_str());
    // if (user.getState() == "online")
    {
        // LOG_INFO << "toid is online";
        LOG_INFO("toid is online\n");
        lock_guard<mutex> lock(_connMutex);
        auto it = _userConnMap.find(toid);
        if (it != _userConnMap.end())
        {
            // LOG_INFO << "transport msg";
            LOG_INFO("transport msg\n");
            // 创建转发json，并进行转发
            // it->second->send(js.dump() + "\n");
            sendMsg(it->second, js.dump(), time);
            return;
        }
    }
    /**
     * 接收用户不在线,储存离线消息
     */
    // _offlineMsgModel.insert(toid, js.dump());
}

void Chatservice::groupChat(const TcpConnectionPtr &conn, json &js, TimeStamp time)
{
    /**
     * msgid, fromid, frommsg, toid, createat, sessiontype
     * 转发内容除多了fromname外一致
     */
    // int userid = js["userid"].get<int>();
    int groupid = js["toid"].get<int>();
    int fromid = js["fromid"].get<int>();
    int64_t createat = js["createat"].get<int64_t>();
    string msg = js["frommsg"];
    vector<GroupUser> users = _groupModel.queryGroupUsers(fromid, groupid);

    _messageModel.insert(-1, fromid, groupid, msg, 1, createat);

    // 获取消息发送者用户名便于显示
    User usr = _userModel.query(fromid);
    string fromname = "";
    if (usr.getId() != -1)
        fromname = usr.getName();
    js["fromname"] = fromname;

    lock_guard<mutex> lock(_connMutex);
    for (GroupUser &user : users)
    {
        int id = user.getId();
        auto it = _userConnMap.find(id);
        /**
         * 判断群内其他用户是否在线
         */
        if (it != _userConnMap.end())
        {

            // it->second->send(js.dump());
            sendMsg(it->second, js.dump(), time);
        }
        // else
        // {
        //     _offlineMsgModel.insert(id, js.dump());
        // }
    }
}

void Chatservice::clientCloseException(const TcpConnectionPtr &conn)
{
    User usr;
    {
        lock_guard<mutex> lock(_connMutex);
        for (auto it = _userConnMap.begin(); it != _userConnMap.end(); it++)
        {
            if (it->second == conn)
            {
                usr.setId(it->first);
                _userConnMap.erase(it);
                break;
            }
        }
    }

    if (usr.getId() != -1)
    {
        usr.setState("offline");
        _userModel.updateState(usr);
    }
}

void Chatservice::addFriend(const TcpConnectionPtr &conn, json &js, TimeStamp time)
{
    /**
     * msgid, id, friendid
     */
    int id = js["id"].get<int>();
    int friendid = js["friendid"].get<int>();

    _friendModel.insert(id, friendid);
}

/**
 * 服务器异常退出，重置客户端状态
 */
void Chatservice::reset()
{
    _userModel.resetState();
}

// void Chatservice::createGroup(const TcpConnectionPtr &conn, json &js, TimeStamp time)
// {
//     /**
//      * msgid, userid, groupname,groupdesc
//      */
//     int userid = js["userid"].get<int>();
//     string name = js["groupname"];
//     string desc = js["groupdesc"];

//     Group group(-1, name, desc);
//     if (_groupModel.createGroup(group))
//     {
//         /**
//          * 存储创始人信息
//          */
//         _groupModel.addGroup(userid, group.getId(), "creator");
//     }
// }

void Chatservice::addGroup(const TcpConnectionPtr &conn, json &js, TimeStamp time)
{
    /**
     * msgid, userid, groupid
     */
    int userid = js["userid"].get<int>();
    int groupid = js["groupid"].get<int>();

    _groupModel.addGroup(userid, groupid, "normal");
}

void Chatservice::heartCheck(const TcpConnectionPtr &conn, json &js, TimeStamp time)
{
    string heartmsg = js["heartMsg"];
    // LOG_INFO << heartmsg;
    LOG_INFO("%s\n", heartmsg.c_str());
    json res;
    res["msgid"] = HEART_MSG;
    res["heartMsg"] = "heart_ok";
    // conn->send(res.dump() + "\n");
    sendMsg(conn, res.dump(), time);
}

void Chatservice::getSessionHistory(const TcpConnectionPtr &conn, json &js, TimeStamp time)
{
    /**
     * 消息获取协议：msgid，userid，sessionid，sessiontype
     * 回复：msgid，errno，messages/errmsg
     */
    int userid = js["userid"].get<int>();
    int sessionid = js["sessionid"].get<int>();
    int sessiontype = js["sessiontype"].get<int>();

    json res;
    res["msgid"] = GET_SESSION_MESSAGE_ACK;
    vector<Message> vec;
    if (!sessiontype)
    {
        /**
         * 获取单聊消息
         */
        vec = _messageModel.queryOne(userid, sessionid);
    }
    else
    {
        /**
         * 获取群聊消息
         */
        vec = _messageModel.queryGroup(sessionid);
    }
    if (!vec.empty())
    {
        vector<string> _res;
        for (Message &msg : vec)
        {
            json js;
            js["toid"] = msg.getToId();
            js["fromid"] = msg.getFromId();
            js["groupid"] = msg.getGroupId();
            js["content"] = msg.getMsg();
            js["state"] = msg.getState();
            js["createat"] = msg.getCreateAt();
            js["fromname"] = msg.getFromName();
            _res.push_back(js.dump());
        }
        res["errno"] = 0;
        res["messages"] = _res;
    }
    else
    {
        res["errno"] = 1;
        res["errmsg"] = "can not get messages!";
        res["messages"] = "";
    }
    // conn->send(res.dump() + "\n");
    sendMsg(conn, res.dump(), time);
}

void Chatservice::getFriends(const TcpConnectionPtr &conn, json &js, TimeStamp time)
{
    /**
     *  好友信息查找协议：msgid，userid，prename
     *  回复：msgid, friends
     */
    int userid = js["userid"].get<int>();
    string prename = js["prename"];

    // LOG_INFO << "queryfriends: " << userid << " " << prename << " " << prename.size();
    LOG_INFO("queryfriends:%d prename:%s prenamesize:%d\n", userid, prename.c_str(), prename.size());

    json res;
    res["msgid"] = FIND_FRIENDS;
    vector<User> friends = _friendModel.query(userid, prename);
    if (!friends.empty())
    {
        vector<string> _res;
        for (User &user : friends)
        {
            json js;
            js["friendid"] = user.getId();
            js["friendname"] = user.getName();
            js["friendstate"] = user.getState();
            _res.push_back(js.dump());
        }
        res["friends"] = _res;
    }
    else
    {
        res["friends"] = "";
    }

    // conn->send(res.dump() + "\n");
    sendMsg(conn, res.dump(), time);
}

void Chatservice::getUsers(const TcpConnectionPtr &conn, json &js, TimeStamp time)
{
    /**
     * 查询用户列表协议: msgid, userid, prename
     * 返回用户列表协议: msgid, users(userid, username, state, isfriend)
     */
    int userid = js["userid"].get<int>();
    string prename = js["prename"];

    vector<FriendUser> users = _userModel.queryUsersByPreName(userid, prename);

    json res;
    res["msgid"] = SEARCH_USERS_ACK;

    if (!users.empty())
    {
        vector<string> _res;
        for (auto &user : users)
        {
            json js;
            js["userid"] = user.getId();
            js["username"] = user.getName();
            js["state"] = user.getState();
            js["isfriend"] = user.getIsFriend();
            _res.push_back(js.dump());
        }
        res["users"] = _res;
    }
    else
    {
        res["users"] = "";
    }

    // conn->send(res.dump() + "\n");
    sendMsg(conn, res.dump(), time);
}

void Chatservice::friendRequest(const TcpConnectionPtr &conn, json &js, TimeStamp time)
{
    /**
     * 发送好友请求协议：msgid, fromid, toid, message
     */
    FriendRequest req;
    req.setFromid(js["fromid"].get<int>());
    req.setToid(js["toid"].get<int>());
    req.setMessage(js["message"]);

    _requestModel.insert(req);
}

void Chatservice::friendRequestReturn(const TcpConnectionPtr &conn, json &js, TimeStamp time)
{
    /**
     *  获取好友请求协议：msgid, userid
     *  返回协议: msgid, reqs(reqid, fromid, fromname, message)
     */
    int userid = js["userid"];
    vector<FriendRequest> reqs = _requestModel.query(userid);

    json res;
    res["msgid"] = GET_FRIEND_REQUEST_ACK;
    vector<string> _reqs;
    if (!reqs.empty())
    {
        for (auto &req : reqs)
        {
            json js;
            js["reqid"] = req.getReqid();
            js["fromid"] = req.getFromid();
            js["fromname"] = req.getFromname();
            js["message"] = req.getMessage();
            _reqs.push_back(js.dump());
        }
        res["reqs"] = _reqs;
    }
    else
    {
        res["reqs"] = "";
    }

    // conn->send(res.dump() + "\n");
    sendMsg(conn, res.dump(), time);
}

// 同意好友请求
void Chatservice::acceptFriendRequest(const TcpConnectionPtr &conn, json &js, TimeStamp time)
{
    /**
     * 同意请求协议为：msgid，reqid，fromid，userid
     * 好友列表通知协议: msgid, friendid, friendname, friendstate
     * 好友请求通知协议即为获取好友请求协议
     */
    // 添加好友数据，通知前端，删除相关请求
    int fromid = js["fromid"].get<int>();
    int userid = js["userid"].get<int>();

    // 添加双向好友关系
    _friendModel.insert(userid, fromid);
    _friendModel.insert(fromid, userid);

    User usr = _userModel.query(fromid);
    User _usr = _userModel.query(userid); // 通知对方
    json res;
    res["msgid"] = NOTICE_FRIENDSLISTCHANGED;
    res["fid"] = usr.getId();
    res["fname"] = usr.getName();
    res["fstate"] = usr.getState();
    // conn->send(res.dump() + "\n");      // 通知当前用户
    sendMsg(conn, res.dump(), time); // 通知当前用户

    auto it = _userConnMap.find(usr.getId()); // 通知对方用户
    if (it != _userConnMap.end())
    {
        json js;
        js["msgid"] = NOTICE_FRIENDSLISTCHANGED;
        js["fid"] = _usr.getId();
        js["fname"] = _usr.getName();
        js["fstate"] = _usr.getState();
        // it->second->send(js.dump() + "\n");
        sendMsg(it->second, js.dump(), time);
    }
    _requestModel.removeAccept(userid, fromid);
}

// 拒绝好友请求
void Chatservice::rejectFriendRequest(const TcpConnectionPtr &conn, json &js, TimeStamp time)
{
    /**
     * 拒绝好友添加协议：msgid, reqid
     */

    _requestModel.removeRefuse(js["reqid"].get<int>());
}

// 创建群聊
void Chatservice::createGroupChat(const TcpConnectionPtr &conn, json &js, TimeStamp time)
{
    /**
     * 创建群聊协议：msgid， userid，groupname，groupdesc，memberids
     * 通知群聊会话改变协议：msgid，groupid，groupname
     */
    string groupname = js["groupname"];
    string groupdesc = js["groupdesc"];
    string memberids = js["memberids"];
    json members_js = json::parse(memberids);
    vector<int> members = members_js.get<vector<int>>();
    // LOG_INFO << "this memberids: ";
    LOG_INFO("this memberids: ");
    for (auto member : members)
    {
        // LOG_INFO << member << " ";
        LOG_INFO("%d ", member);
    }
    LOG_INFO("\n");
    int userid = js["userid"].get<int>();

    // 新建一个群聊
    Group group;
    group.setName(groupname);
    group.setDesc(groupdesc);
    _groupModel.createGroup(group);

    // 添加群聊成员,并进行通知
    json res;
    res["msgid"] = NOTICE_GROUPLISTCHANGED;
    res["groupid"] = group.getId();
    res["groupname"] = groupname;
    _groupModel.addGroup(userid, group.getId(), "creator");
    // conn->send(res.dump() + "\n");
    sendMsg(conn, res.dump(), time);
    for (const auto &id : members)
    {
        // LOG_INFO << "thid id: " << id;
        LOG_INFO("thid id: %d\n", id);
        _groupModel.addGroup(id, group.getId(), "normal");
        auto it = _userConnMap.find(id);
        if (it != _userConnMap.end())
        {
            // it->second->send(res.dump() + "\n");
            sendMsg(it->second, res.dump(), time);
        }
    }
}

void Chatservice::sendMsg(const TcpConnectionPtr &conn, const string &msgStr, TimeStamp time)
{
    /**
     * 分片协议：message_size + js{chunk_id, chunk_count, message}
     */
    // 发送消息之前先加密
    AesGcmManager aesMan = getAesOfConn(conn);
    string _msgStr = aesMan.encrypt(msgStr);
    _msgStr = base64_encode(_msgStr);
    
    uint32_t len = _msgStr.size();
    int chunk_len = 16 * 1024; //  每个消息分片16kb
    if (chunk_len >= len)
    {
        json js;
        // js["message_size"] = len;
        js["chunk_id"] = 1;
        js["chunk_count"] = 1;
        js["message"] = _msgStr;
        uint32_t message_size = htonl(js.dump().length());
        string header(reinterpret_cast<char *>(&message_size), 4);
        fprintf(stdout, "Msg: %s\n", js.dump().c_str());
        fprintf(stdout, "Len: %d\n", js.dump().length());
        
        lock_guard<mutex> lock(_sendMutex);
        conn->send(header + js.dump()); // 先传长度后传送消息主体
        fprintf(stdout, "message 发送成功\n");
    }
    else
    {
        int chunk_count = (len + chunk_len - 1) / chunk_len;
        uint32_t _begin = 0;
        lock_guard<mutex> lock(_sendMutex);
        for (size_t i = 1; i <= chunk_count; i++)
        {
            json js;
            js["chunk_id"] = i;
            js["chunk_count"] = chunk_count;
            if (i != chunk_count)
            {
                js["message"] = _msgStr.substr(_begin, chunk_len);
                _begin += chunk_len;
            }
            else
            {
                js["message"] = _msgStr.substr(_begin, len - _begin);
            }
            uint32_t message_size = htonl(js.dump().length());
            string header(reinterpret_cast<char *>(&message_size), 4);
            fprintf(stdout, "Msg: %s\n", js.dump().c_str());
            fprintf(stdout, "Len: %d\n", js.dump().length());
            conn->send(header + js.dump());
        }
    }
}

void Chatservice::setRsaAesKey(const TcpConnectionPtr &conn, json &js, TimeStamp time)
{
    /**
     * 设置aes密钥协议：msgid， key
     */
    fprintf(stdout, "正在解密aes密钥\n");

    // 先解码Base64还原原二进制数据再转换为字符串
    string key = js["key"];
    string binary_key = base64_decode(key);
    if (binary_key.empty())
    {
        fprintf(stderr, "错误：Base64 解码失败\n");
        return;
    }
    fprintf(stdout, "Base64 解码后长度: %zu 字节\n", binary_key.length());

    // 3. 打印十六进制以便调试
    fprintf(stdout, "解码后的二进制数据（十六进制）:\n");
    for (size_t i = 0; i < binary_key.length(); i++)
    {
        fprintf(stdout, "%02X ", (unsigned char)binary_key[i]);
        if ((i + 1) % 16 == 0)
            fprintf(stdout, "\n");
    }

    // 解码出真正的aes密钥明文
    string aeskey = _decryptManager.decryptWithPrivateKey(binary_key);
    if (aeskey.empty())
    {
        fprintf(stderr, "错误：RSA 解密失败\n");
        return;
    }

    // 5. 打印 AES 密钥
    fprintf(stdout, "AES 密钥（十六进制）: ");
    for (size_t i = 0; i < aeskey.length(); i++)
    {
        fprintf(stdout, "%02X ", (unsigned char)aeskey[i]);
    }
    fprintf(stdout, "\n");

    // 创建aes管理，并设置connaesmap
    AesGcmManager aesMan(aeskey);
    _connAesMap.insert({conn, aesMan});
}

bool Chatservice::findAesOfConn(const TcpConnectionPtr &conn)
{
    auto it = _connAesMap.find(conn);
    if (it == _connAesMap.end())
        return false;
    return true;
}

AesGcmManager Chatservice::getAesOfConn(const TcpConnectionPtr& conn) {
    auto it = _connAesMap.find(conn);
    return it->second;
}
