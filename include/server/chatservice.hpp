#ifndef CHATSERVICE_HPP
#define CHATSERVICE_HPP
#include <tcp_connection.hpp>
#include "callbacks.hpp"
#include <unordered_map>
#include <functional>
#include "usermodel.hpp"
#include "friendmodel.hpp"
#include "groupmodel.hpp"
#include "messagemodel.hpp"
#include "requestmodel.hpp"
#include "rsa/rsa.hpp"
#include "aesgcm/aesgcm.hpp"
#include "redis/redis.hpp"
#include <mutex>
#include <atomic>

using namespace std;

#include <json.hpp>
using json = nlohmann::json;

using MsgHandler = function<void(const TcpConnectionPtr &conn, json &js, TimeStamp time)>;

/**
 * 聊天服务器业务类
 */
class Chatservice
{
public:
    static Chatservice* getInstance();
    void login(const TcpConnectionPtr &conn, json &js, TimeStamp time);
    void loginOut(const TcpConnectionPtr &conn, json &js, TimeStamp time);
    void reg(const TcpConnectionPtr &conn, json &js, TimeStamp time);
    void clientCloseException(const TcpConnectionPtr &conn);
    void oneChat(const TcpConnectionPtr& conn, json &js, TimeStamp time);
    void addFriend(const TcpConnectionPtr &conn, json &js, TimeStamp time);
    // 群组相关操作
    void createGroupChat(const TcpConnectionPtr& conn, json &js, TimeStamp time);
    void addGroup(const TcpConnectionPtr& conn, json &js, TimeStamp time);
    void groupChat(const TcpConnectionPtr& conn, json &js, TimeStamp time);
    // 心跳包检测
    void heartCheck(const TcpConnectionPtr& conn, json &js, TimeStamp time);
    // 获取历史消息
    void getSessionHistory(const TcpConnectionPtr& conn, json &js, TimeStamp time);
    // 获取好友列表
    void getFriends(const TcpConnectionPtr& conn, json &js, TimeStamp time);
    // 获取指定用户列表
    void getUsers(const TcpConnectionPtr& conn, json &js, TimeStamp time);
    // 存储好友请求
    void friendRequest(const TcpConnectionPtr& conn, json &js, TimeStamp time);
    // 获取好友请求列表
    void friendRequestReturn(const TcpConnectionPtr& conn, json &js, TimeStamp time);
    // 同意好友请求
    void acceptFriendRequest(const TcpConnectionPtr& conn, json &js, TimeStamp time);
    // 拒绝好友请求
    void rejectFriendRequest(const TcpConnectionPtr& conn, json &js, TimeStamp time);
    // 设置aes密钥
    void setRsaAesKey(const TcpConnectionPtr& conn, json &js, TimeStamp time);
    // 根据连接查找是否存在对应的aes密钥
    bool findAesOfConn(const TcpConnectionPtr& conn);
    // 获取连接对应aes密钥
    AesGcmManager getAesOfConn(const TcpConnectionPtr& conn);
    // 从redis消息队列中获取订阅的消息
    void handleRedisSubscribeMessage(int, string);
    
    MsgHandler getMsgHandler(int msgid);
    void reset();
private:
    /**
     * 单例模式，构造函数私有化，不允许外部构造
     */
    Chatservice();

    void sendMsg(const TcpConnectionPtr& conn, const string &msgStr, TimeStamp time);

    // 存储消息id与对应处理器映射表，由于是在程序一开始就初始化完毕，不用考虑线程安全问题
    unordered_map<int, MsgHandler> _msgHandlerMap;
    // 在线用户的通信连接映射表，在程序执行过程中存在改变，需要注意线程安全的问题
    unordered_map<int, TcpConnectionPtr> _userConnMap;  
    // 存储每个连接通信所需的aes密钥
    unordered_map<TcpConnectionPtr, AesGcmManager> _connAesMap;
    // 互斥锁，保证_userConnMap线程安全
    mutex _connMutex;
    // 互斥锁，确保分片消息能整体发送
    mutex _sendMutex;
    // 记录发送消息全局seq，维护消息有序性
    atomic<int> seq;

    UserModel _userModel;
    FriendModel _friendModel;
    GroupModel _groupModel;
    MessageModel _messageModel;
    RequestModel _requestModel;
    RsaKeyManager _decryptManager;

    Redis redis;
};

#endif