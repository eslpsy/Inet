#include <list>
#include <algorithm>
#include <memory>
#include <string>
#include "../../src/net/TcpServer.h"
#include "../../src/net/Buffer.h"
#include "../../src/net/TcpConnection.h"
#include "../../src/net/EventLoop.h"

using namespace std;
using namespace inet;

list<TcpConnectionPtr> client_list;

class ClientContext
{
    public:
        ClientContext() : status(kNeedAccount)
        {
        }

        ~ClientContext()
        {
        }

        enum Status
        {
            kNeedAccount, kNeedPassword, kAllGot
        };

        Status getStatus() const
        {
            return status;
        }

        string getAccount() const
        {
            return account;
        }

        string getPassword() const
        {
            return passwd;
        }

        void setAccount(string ac)
        {
            account = ac;
        }

        void setPassword(string ps)
        {
            passwd = ps;
        }
        
        void setStatus(Status s)
        {
            status = s;
        }

    private:
        string account;
        string passwd;
        Status status;
};

void onConnection(const TcpConnectionPtr& conn)
{
    if(conn->connected())
    {
        conn->setContext(ClientContext());
        client_list.push_back(conn);
        conn->send(string("Please enter your account\n"));
    }
    else
    {
        const ClientContext* cc = boost::any_cast<ClientContext>(conn->getContext());
        string buf("System Message : user ");
        buf += cc->getAccount();
        buf += " disconnected\n";
        for(auto iter = client_list.begin(); iter != client_list.end(); ++iter)
        {
            if(*iter != conn)
            {
                (*iter)->send(buf);
            }
        }
        client_list.remove(conn);
    }
}

void onMessage(const TcpConnectionPtr& conn, Buffer* buf, Timestamp time)
{
    ClientContext* cc = boost::any_cast<ClientContext>(conn->getMutableContext());
    ClientContext::Status status = cc->getStatus();

    if(status == ClientContext::kAllGot)
    {
        string time_str(time.toReadString());
        time_str += "\nuser:";
        time_str += cc->getAccount();
        time_str += "\ncontent:";
    
        for(auto iter = client_list.begin(); iter != client_list.end(); ++iter)
        {
            if(*iter != conn)
            {
                (*iter)->send(time_str);
                (*iter)->send(*buf);
            }
        }
        buf->retrieveAll();
    }
    else if(status == ClientContext::kNeedAccount)
    {
        string account = buf->retrieveAsString();
        cc->setAccount(account.substr(0, account.size() - 2));
        cc->setStatus(ClientContext::kNeedPassword);
        conn->send("Please enter your password:\n");
    }
    else if(status == ClientContext::kNeedPassword)
    {
        string passwd = buf->retrieveAsString();
        cc->setPassword(passwd.substr(0, passwd.size() - 2));
        cc->setStatus(ClientContext::kAllGot);

        string temp_buf;
        temp_buf.reserve(1024);
        temp_buf.append("Welcome to chat room, ");
        temp_buf.append(cc->getAccount());
        temp_buf.append("\n\n");
        conn->send(temp_buf);
    }
}

int main(int argc, char** argv)
{
    InetAddress addr(5555);
    EventLoop loop;
    TcpServer char_room(&loop, addr);
   
    char_room.setConnectionCallback(onConnection);
    char_room.setMessageCallback(onMessage);
    char_room.start();
    loop.loop();
}
