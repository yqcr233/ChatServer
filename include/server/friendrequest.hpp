#ifndef FRIENDREQUEST_HPP
#define FRIENDREQUEST_HPP
#include <string>
using namespace std;

class FriendRequest
{
public:
    FriendRequest(int reqid = -1, int fromid = -1, int toid = -1, string fromname = "", string message = ""){
        this->reqid = reqid;
        this->fromid = fromid;
        this->toid = toid;
        this->fromname = fromname;
        this->message = message;
    }

    void setReqid(int reqid) {this->reqid = reqid;}
    void setFromid(int fromid) {this->fromid = fromid;}
    void setToid(int toid) {this->toid = toid;}
    void setFromname(string fromname) {this->fromname= fromname;}
    void setMessage(string message) {this->message = message;}

    int getReqid() {return reqid;}
    int getFromid() {return fromid;}
    int getToid() {return toid;}
    string getFromname() {return fromname;}
    string getMessage() {return message;}

private:
    int reqid;
    int fromid;
    int toid;
    string fromname;
    string message;
};

#endif