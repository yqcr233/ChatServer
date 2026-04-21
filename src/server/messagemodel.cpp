#include "message.hpp"
#include "timeutil.hpp"
#include "sessioninfo.hpp"
#include "messagemodel.hpp"
#include "timeutil.hpp"
#include "db.h"

void MessageModel::insert(int toid, int fromid, int groupid, string msg, int state, int64_t createAt)
{
    string _createAt = TimeUtil::msToMysqlTimestamp(createAt);
    fprintf(stdout, "createAt: %s\n", _createAt.c_str());
    char sql[1024];
    sprintf(sql, "insert into message(to_id, from_id, group_id, content, state, created_at) values(%d, %d, %d,'%s', %d, '%s')", \
            toid, fromid, groupid, msg.c_str(), state, _createAt.c_str());

    MySQL mysql;
    if (mysql.connect())
    {
        mysql.update(sql);
    }
}

vector<SessionInfo> MessageModel::querySessions(int userid)
{
    char sql[2048];
    sprintf(sql, " (SELECT 'private' as chat_type,  \
            CASE \
                WHEN m.to_id = %d THEN m.from_id \
                ELSE m.to_id \
            END as session_id, \
            u.name as session_name, \
            u.state as is_online,   \
            MAX(m.created_at) as latest_time, \
            SUBSTRING( \
                (SELECT m2.content  \
                FROM message m2  \
                WHERE m2.id = MAX(m.id) \
                LIMIT 1), 1, 50) as last_message, \
            SUM(CASE WHEN m.state = 0 THEN 1 ELSE 0 END) as unread_count \
        FROM message m \
        LEFT JOIN user u ON u.id = CASE  \
            WHEN m.to_id = %d THEN m.from_id \
            ELSE m.to_id \
        END \
        WHERE m.group_id = -1   \
        AND (m.to_id = %d OR m.from_id = %d)   \
        GROUP BY  \
            CASE  \
                WHEN m.to_id = %d THEN m.from_id \
                ELSE m.to_id \
            END, \
            u.name, \
            u.state) \
        union all \
        (SELECT \
        'group' as chat_type,  \
        m.group_id as session_id, \
        g.groupname as session_name,  \
        'online' as is_online,  \
        MAX(m.created_at) as latest_time,  \
        SUBSTRING(\
            (SELECT m2.content \
            FROM message m2 \
            WHERE m2.id = MAX(m.id)\
            LIMIT 1), 1, 50) as last_message,\
        SUM(CASE WHEN m.state = 0 THEN 1 ELSE 0 END) as unread_count\
        FROM message m\
        LEFT JOIN allgroup g ON g.id = m.group_id\
        WHERE m.group_id != -1  and m.group_id in (select groupid from groupuser where userid = %d)\
        GROUP BY m.group_id, g.groupname)", userid, userid, userid, userid, userid, userid);

    vector<SessionInfo> vec;
    MySQL mysql;
    
    if(mysql.connect()) {
        MYSQL_RES* res = mysql.query(sql);
        if(res != nullptr) {
            MYSQL_ROW row;
            while((row = mysql_fetch_row(res)) != nullptr) {
                SessionInfo info;
                info.setChatType(row[0]);
                info.setSessionId(atoi(row[1]));
                info.setSessionName(row[2]);
                info.setIsOnline(row[3]);
                info.setLatestTime(TimeUtil::mysqlTimestampToMs(row[4]));
                info.setLastMessage(row[5]);
                info.setUnreadCount(atoi(row[6]));
                vec.push_back(info);
            }
            mysql_free_result(res);
        }
    }
    return vec;
}
/**
 * 请求会话历史记录协议： msgid，userid，sessionid，sessiontype
 */
vector<Message> MessageModel::queryOne(int userid, int sessionid)
{
    fprintf(stdout, "queryOne %d %d\n", userid, sessionid);
    char sql[1024];
    sprintf(sql, "select m.*, u.name as from_name from message m left join user u on u.id = m.from_id where group_id = -1 and \
        ((to_id=%d and from_id=%d) or (to_id=%d and from_id=%d))", userid,sessionid,sessionid,userid);
     
    vector<Message> vec;
    MySQL mysql;
    if(mysql.connect()) {
        MYSQL_RES* res = mysql.query(sql);
        if(res != nullptr) {
            MYSQL_ROW row;
            while((row = mysql_fetch_row(res)) != nullptr) {
                Message msg;
                msg.setToId(atoi(row[1]));
                msg.setFromId(atoi(row[2]));
                msg.setGroupId(atoi(row[3]));
                msg.setMsg(row[4]);
                msg.setState(atoi(row[5]));
                msg.setCreateAt(TimeUtil::mysqlTimestampToMs(row[6]));
                msg.setFromName(row[7]);
                vec.push_back(msg);

                fprintf(stdout, "%ld\n", TimeUtil::mysqlTimestampToMs(row[6]));
            }
            mysql_free_result(res);
        }
    }
    return vec;
}

vector<Message> MessageModel::queryGroup(int sessionid)
{
    fprintf(stdout, "queryGroup %d\n", sessionid);
    char sql[1024];
    sprintf(sql, "select m.*, u.name from message m left join user u on u.id=m.from_id where group_id = %d", sessionid);

    vector<Message> vec;
    MySQL mysql;
    if(mysql.connect()) {
        MYSQL_RES* res = mysql.query(sql);
        if(res !=nullptr) {
            MYSQL_ROW row;
            while((row=mysql_fetch_row(res)) != nullptr) {
                Message msg;
                msg.setToId(atoi(row[1]));
                msg.setFromId(atoi(row[2]));
                msg.setGroupId(atoi(row[3]));
                msg.setMsg(row[4]);
                msg.setState(atoi(row[5]));
                msg.setCreateAt(TimeUtil::mysqlTimestampToMs(row[6]));
                msg.setFromName(row[7]);
                vec.push_back(msg);
            }
            mysql_free_result(res);
        }
    }
    return vec;
}
