#ifndef __INET_ACCEPTOR_H
#define __INET_ACCEPTOR_H

#include <functional>
#include "../base/Noncopyable.h"
#include "Socket.h"
#include "Channel.h"

namespace inet
{
    class EventLoop;
    class InetAddress;

    class Acceptor : Noncopyable
    {
        public:
            typedef std::function<void (int sockfd, const InetAddress&)> NewConnectionCallback;
            
            Acceptor(EventLoop* loop, const InetAddress& listenAddr, bool reuseAddr = true);

            void setNewConnectionCallback(const NewConnectionCallback& cb)
            {
                newConnectionCallback_ = cb;
            }

            bool listening() const
            {
                return listening_;
            }

            void listen();

        private:
            void handleRead();

            EventLoop* loop_;
            Socket acceptSocket_;
            Channel acceptChannel_;
            NewConnectionCallback newConnectionCallback_;
            bool listening_;
    };
}
#endif
