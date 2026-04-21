#include "requestmodel.hpp"
#include "db.hpp"

/**
 * 插入一条好友请求
 */
void RequestModel::insert(FriendRequest req)
{
    char sql[1024];
    sprintf(sql, "insert into friendrequests(fromid, toid, message) values(%d, %d, '%s')", req.getFromid(), req.getToid(), req.getMessage().c_str());

    MySQL mysql;
    
    mysql.update(sql);
}

/**
 * 查询指定用户的所有好友请求
 */
vector<FriendRequest> RequestModel::query(int userid)
{
    char sql[1024];
    sprintf(sql, "select r.id, r.fromid, u.name, r.message from friendrequests r join user u on r.fromid=u.id where r.toid=%d", userid);

    MySQL mysql;
    vector<FriendRequest> vec;

    MYSQL_RES *res = mysql.query(sql);
    if (res != nullptr)
    {
        MYSQL_ROW row;
        while ((row = mysql_fetch_row(res)) != nullptr)
        {
            FriendRequest req;
            req.setReqid(atoi(row[0]));
            req.setFromid(atoi(row[1]));
            req.setFromname(row[2]);
            req.setMessage(row[3]);
            vec.push_back(req);
        }
        mysql_free_result(res);
        return vec;
    }
    return vec;
}

/**
 * 同意好友请求时，移除所有该用户的好友请求
 */
void RequestModel::removeAccept(int userid, int fromid)
{
    char sql[1024];
    sprintf(sql, "delete from friendrequests where toid = %d and fromid=%d", userid, fromid);

    MySQL mysql;

    mysql.update(sql);
}

/**
 * 拒绝好友请求时，只移除该条好友请求
 */
void RequestModel::removeRefuse(int requestid)
{
    char sql[1024];
    sprintf(sql, "delete from friendrequests where id = %d", requestid);

    MySQL mysql;
    
    mysql.update(sql);
}