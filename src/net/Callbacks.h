#ifndef __INET_CALLBACKS_H
#define __INET_CALLBACKS_H

#include <functional>
#include <memory>
#include "../base/Timestamp.h"

namespace inet
{
    typedef std::function<void ()> TimerCallback;
    class TcpConnection;
    class Buffer;
    typedef std::shared_ptr<TcpConnection> TcpConnectionPtr;
    typedef std::function<void (const TcpConnectionPtr&)> ConnectionCallback;
    typedef std::function<void (const TcpConnectionPtr&, Buffer* buf, Timestamp)> MessageCallback;
    typedef std::function<void (const TcpConnectionPtr&)> CloseCallback;
}

#endif
