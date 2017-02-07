#include <fcntl.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>
#include <stdlib.h>
#include <strings.h>
#include <errno.h>
#include <stdio.h>
#include "SocketsOps.h"

using namespace inet;

namespace
{
    typedef struct sockaddr SA;

    const SA* sockaddr_cast(const struct sockaddr_in* addr)
    {
        return static_cast<const SA*>(static_cast<const void*>(addr));
    }

    SA* sockaddr_cast(struct sockaddr_in* addr)
    {
        return static_cast<SA*>(static_cast<void*>(addr));
    }

    void setNonBlockAndCloseOnExec(int sockfd)
    {
        int flags = ::fcntl(sockfd, F_GETFL, 0);
        flags |= O_NONBLOCK;
        int ret = ::fcntl(sockfd, F_SETFL, flags);
        flags = ::fcntl(sockfd, F_GETFL, 0);
        flags |= O_CLOEXEC;
        ret = ::fcntl(sockfd, F_SETFL, flags);
    }
}

int sockets::createNonblockOrDie()
{
    int sockfd = ::socket(AF_INET, SOCK_STREAM | SOCK_NONBLOCK | SOCK_CLOEXEC,
                         IPPROTO_TCP);
    if(sockfd < 0)
        abort();
    return sockfd;
}

void sockets::bindOrDie(int sockfd, const struct sockaddr_in& addr)
{
    int ret = ::bind(sockfd, sockaddr_cast(&addr), sizeof(addr));
    if(ret < 0)
        abort();
}

void sockets::listenOrDie(int sockfd)
{
    int ret = ::listen(sockfd, SOMAXCONN);
    if(ret < 0)
        abort();
}

int sockets::accept(int sockfd, struct sockaddr_in* addr)
{
    socklen_t addrlen = sizeof(*addr);
    int connfd = ::accept4(sockfd, sockaddr_cast(addr), &addrlen, SOCK_NONBLOCK | SOCK_CLOEXEC);

    if(connfd < 0)
    {
        int saveErrno = errno;
        switch(saveErrno)
        {
                case EAGAIN:
            case ECONNABORTED:
            case EINTR:
            case EPROTO:
            case EPERM:
            case EMFILE:
                errno = saveErrno;
                break;
            case EBADF:
            case EFAULT:
            case EINVAL:
            case ENFILE:
            case ENOBUFS:
            case ENOMEM:
            case ENOTSOCK:
            case EOPNOTSUPP:
                abort();
                break;
            defalut:
                abort();
        }
    }

    return connfd;
}

void sockets::close(int sockfd)
{
    if(::close(sockfd) < 0)
        abort();
}

void sockets::shutdownWrite(int sockfd)
{
    if(::shutdown(sockfd, SHUT_WR) < 0)
        abort();
}

void sockets::toHostPort(char* buf, size_t size, const struct sockaddr_in& addr)
{
    char host[INET_ADDRSTRLEN] = "INVALID";
    ::inet_ntop(AF_INET, &addr.sin_addr, host, sizeof(host));
    uint16_t port = networkToHost16(addr.sin_port);
    snprintf(buf, size, "%s:%u", host, port);
}

void sockets::fromHostPort(const char *ip, uint16_t port, struct sockaddr_in* addr)
{
    addr->sin_family = AF_INET;
    addr->sin_port = hostToNetwork16(port);
    if(::inet_pton(AF_INET, ip, &addr->sin_addr) <= 0)
    {
        abort();
    }
}

struct sockaddr_in sockets::getLocalAddress(int sockfd)
{
    struct sockaddr_in localaddr;
    bzero(&localaddr, sizeof(localaddr));
    socklen_t addrlen = sizeof(localaddr);
    if(::getsockname(sockfd, sockaddr_cast(&localaddr), &addrlen) < 0)
        abort();
    return localaddr;
}

struct sockaddr_in sockets::getPeerAddress(int sockfd)
{
    struct sockaddr_in peeraddr;
    bzero(&peeraddr, sizeof(peeraddr));
    socklen_t addrlen = sizeof(peeraddr);
    if(::getpeername(sockfd, sockaddr_cast(&peeraddr), &addrlen) < 0)
        abort();
    return peeraddr;
}

int sockets::connect(int sockfd, const struct sockaddr_in& serveraddr)
{
   return ::connect(sockfd, sockaddr_cast(&serveraddr), sizeof(serveraddr));
}

int sockets::getSocketError(int sockfd)
{
    int optval;
    socklen_t optlen = sizeof(optval);

    if(::getsockopt(sockfd, SOL_SOCKET, SO_ERROR, &optval, &optlen) < 0)
        return errno;
    else
        return optval;
}

bool sockets::isSelfConnect(int sockfd)
{
    struct sockaddr_in localAddr = getLocalAddress(sockfd);
    struct sockaddr_in peerAddr = getPeerAddress(sockfd);
    return localAddr.sin_port == peerAddr.sin_port && localAddr.sin_addr.s_addr == peerAddr.sin_addr.s_addr;
}
