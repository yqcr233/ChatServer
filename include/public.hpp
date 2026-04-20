#ifndef PUBLIC_HPP
#define PUBLIC_HPP
/**
 * service和client的公共文件
 */

enum EnMsgType{
    LOGIN_MSG = 1,
    LOGIN_MSG_ACK,
    LOGINOUT_MSG,
    REG_MSG,
    REG_MSG_ACK,
    ONE_CHAT_MSG,    // 单聊信息
    ADD_FRIEND_MSG,  // 添加好友消息

    CREATE_GROUP_MSG,
    ADD_GROUP_MSG,
    GROUP_CHAT_MSG,

    HEART_MSG,       // 心跳包检测信息
    GET_SESSION_MESSAGE, // 获取会话历史信息
    GET_SESSION_MESSAGE_ACK,
    FIND_FRIENDS,    // 查找好友信息

    NOTICE_FRIENDSTATE,   // 实时通信好友状态信息改变
    SEARCH_USERS,         // 请求用户信息
    SEARCH_USERS_ACK,     // 请求用户返回信息

    FRIEND_REQUEST,       //  发送添加好友请求
    GET_FRIEND_REQUEST,       //  获取添加好友请求
    GET_FRIEND_REQUEST_ACK,       //  获取添加好友请求返回
    REMOVE_REQUEST_ACCEPT,             // 接收添加好友请求
    // NOTICE_REMOVE_REQUEST_ACCEPT,      // 接收移除后通知
    REMOVE_REQUEST_REFUSE,             // 拒绝添加好友请求
    NOTICE_FRIENDSLISTCHANGED,         // 通知好友列表更新
    NOTICE_GROUPLISTCHANGED,           // 通知群聊列表更新

    SETRSAAESKEY,                      //连接成功后设置在服务端设置该连接的aes密钥
};

#endif