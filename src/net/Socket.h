#ifndef __INET_SOCKET_H
#define __INET_SOCKET_H

#include "../base/Noncopyable.h"

namespace inet
{
    class InetAddress;

    class Socket : Noncopyable
    {
        public:
            explicit Socket(int sockfd) : sockfd_(sockfd)
            {
            }

            ~Socket();

            int fd() const
            {
                return sockfd_;
            }

            void bindAddress(const InetAddress& localaddr);

            void listen();

            int accept(InetAddress* peeraddr);

            void setReuseAddr(bool on);

        private:
            const int sockfd_;
    };
}

#endif
