#ifndef __INET_CHATROOM_H
#define __INET_CHATROOM_H

#include "../TcpServer.h"
#include "../Buffer.h"
#include "../TcpConnection.h"
#include "../InetAddress.h"

#include <list>
#include <string>

using std::list;
using std::string;
using namespace inet;

class ChatRoom
{
    public:
        ChatRoom(EventLoop* loop, InetAddress addr);

        ~ChatRoom();

        void start();

        void onConnectionCallback(const TcpConnectionPtr& conn);
        
        void onMessageCallback(const TcpConnectionPtr& conn, Buffer* buf, Timestamp time); 

        void newUserCallback(const TcpConnectionPtr& conn, Timestamp time, string name);
        
        void sendToAllUser(const char* meesage, ssize_t size);

        void sendToAllUserExcept(const TcpConnectionPtr& conn, const char* message, ssize_t size);

    private:
        list<TcpConnectionPtr> _client_list;
        TcpServer _server;
};

#endif
