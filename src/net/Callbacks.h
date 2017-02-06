#ifndef __INET_CALLBACKS_H
#define __INET_CALLBACKS_H

#include <functional>
#include <memory>

namespace inet
{
    typedef std::function<void ()> TimerCallback;
    class TcpConnection;
    typedef std::shared_ptr<TcpConnection> TcpConnectionPtr;
    typedef std::function<void (const TcpConnectionPtr&)> ConnectionCallback;
    typedef std::function<void (const TcpConnectionPtr&, const char* data, ssize_t len)> MessageCallback;
}

#endif
