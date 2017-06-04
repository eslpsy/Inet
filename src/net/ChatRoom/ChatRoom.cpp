#include "ChatRoom.h"
#include "ClientContext.h"
#include <functional>
#include <boost/any.hpp>
#include <algorithm>
#include <cctype>
#include <cstring>

using std::remove_if;

namespace
{
    bool wrapIsSpace(char c)
    {
        return isspace(c);
    }
}

ChatRoom::ChatRoom(EventLoop* loop, InetAddress addr) : _server(loop, addr)
{
    _server.setConnectionCallback(std::bind(&ChatRoom::onConnectionCallback, this,
                std::placeholders::_1));
    _server.setMessageCallback(std::bind(&ChatRoom::onMessageCallback, this,
                std::placeholders::_1, std::placeholders::_2, std::placeholders::_3));
}

ChatRoom::~ChatRoom()
{
}

void ChatRoom::start()
{
    _server.start();
}

void ChatRoom::onConnectionCallback(const TcpConnectionPtr& conn)
{
    if(conn->connected())
    {
        conn->setContext(ClientContext());
        _client_list.push_back(conn);
    }
    else
    {
        char buf[1024] = {0};
        const ClientContext *cc = boost::any_cast<ClientContext>(conn->getContext());
        string name = cc->getName();

        snprintf(buf, sizeof(buf), "******\n%s\nSystem Message : User %s is disconned\n"
                "******\n", Timestamp::now().toReadString().c_str(), name.c_str());
        _client_list.remove(conn);
        sendToAllUser(buf, strlen(buf));
    }
}

void ChatRoom::sendToAllUser(const char* meesage, ssize_t size)
{
    for(auto iter = _client_list.begin(); iter != _client_list.end(); ++iter)
    {
        (*iter)->send(meesage, size);
    }
}

void ChatRoom::sendToAllUserExcept(const TcpConnectionPtr& conn, const char* message, ssize_t size)
{
    for(auto iter = _client_list.begin(); iter != _client_list.end(); ++iter)
    {
        if(*iter != conn)
            (*iter)->send(message, size);
    }
}

void ChatRoom::newUserCallback(const TcpConnectionPtr& conn, Timestamp time, string name)
{
    char message[1024] = {0};

    snprintf(message, sizeof(message), "******\n%s\nSystem Message : New User %s is online\n******\n",
            time.toReadString().c_str(), name.c_str());
    for(auto iter = _client_list.begin(); iter != _client_list.end(); ++iter)
    {
        if(*iter != conn)
        {
            (*iter)->send(message, strlen(message));
        }
    }
}

void ChatRoom::onMessageCallback(const TcpConnectionPtr& conn, Buffer* buf, Timestamp time)
{
    ClientContext *cc = boost::any_cast<ClientContext>(conn->getMutableContext());
    if(!cc->hasGotName())
    {
        string name = buf->retrieveAsString();
        buf->retrieveAll();
        name.erase(remove_if(name.begin(), name.end(), wrapIsSpace), name.end());
        cc->setName(name);
        cc->setState(ClientContext::kGotAll);
        newUserCallback(conn, time, name);
    }
    else if(cc->hasGotAll())
    {
        const char* temp_msg1 = "------\n";
        const char* temp_msg2 = "\nUser : ";
        const char* temp_msg3 = "\nContent : ";
        Buffer message;
        message.append(temp_msg1, strlen(temp_msg1));
        message.append(time.toReadString());
        message.append(temp_msg2, strlen(temp_msg2));
        message.append(cc->getName());
        message.append(temp_msg3, strlen(temp_msg3));
        message.append(buf->retrieveAsString());
        message.append(temp_msg1, strlen(temp_msg1));

        sendToAllUserExcept(conn, message.peek(), message.readableBytes());
        
        /*for(auto iter = _client_list.begin(); iter != _client_list.end(); ++iter)
        {
            if(conn != (*iter))
            {
                (*iter)->send(cc->getName());
                (*iter)->send(*buf);
            }
        }*/
        buf->retrieveAll();
    }
}
