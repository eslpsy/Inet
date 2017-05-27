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

void onConnection(const TcpConnectionPtr& conn)
{
    if(conn->connected())
    {
        client_list.push_back(conn);
    }
    else
    {
        for(auto iter = client_list.begin(); iter != client_list.end(); ++iter)
        {
            if(*iter != conn)
            {
                (*iter)->send("Someone disconnected\n");
            }
        }
        client_list.remove(conn);
    }
}

void onMessage(const TcpConnectionPtr& conn, Buffer* buf, Timestamp time)
{
    string time_str(time.toReadString());
    time_str += ":";
    
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
