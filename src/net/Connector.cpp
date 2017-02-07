#include <functional>
#include <stdio.h>
#include <errno.h>
#include "Connector.h"
#include "Channel.h"
#include "EventLoop.h"
#include "SocketsOps.h"

using namespace inet;

const int Connector::kMaxRetryMs = 30 * 1000;
const int Connector::kInitRetryMs = 500;

Connector::Connector(EventLoop* loop, const InetAddress& serverAddr)
 : loop_(loop),
   serverAddr_(serverAddr),
   connect_(false),
   state_(kDisconnected),
   retryDelayMs_(kInitRetryMs)
{
}

Connector::~Connector()
{
}

void Connector::start()
{
    connect_ = true;
    loop_->runInLoop(std::bind(&Connector::startInLoop, this));
}

void Connector::startInLoop()
{
    loop_->assertInLoopThread();
    assert(state_ == kDisconnected);
    if(connect_)
    {
        connect();
    }
    else
    {
        // LOG
    }
}

void Connector::connect()
{
    int sockfd = sockets::createNonblockOrDie();
    int retVal = sockets::connect(sockfd, serverAddr_.getSockAddrInet());
    int saveErrno = (retVal == 0)? 0 : errno;
    switch(saveErrno)
    {
        case 0:
        case EINPROGRESS:
        case EINTR:
        case EISCONN:
            connecting(sockfd);
            break;

        case EAGAIN:
        case EADDRINUSE:
        case EADDRNOTAVAIL:
        case ECONNREFUSED:
        case ENETUNREACH:
            retry(sockfd);
            break;

        case EACCES:
        case EPERM:
        case EAFNOSUPPORT:
        case EALREADY:
        case EBADF:
        case EFAULT:
        case ENOTSOCK:
            sockets::close(sockfd);
            break;
       
       default:
            sockets::close(sockfd);
            break;
    }
}

void Connector::restart()
{
    loop_->assertInLoopThread();
    setState(kDisconnected);
    retryDelayMs_ = kInitRetryMs;
    connect_ = true;
    startInLoop();
}

void Connector::connecting(int sockfd)
{
    setState(kConnecting);
    assert(!channel_);
    channel_.reset(new Channel(loop_, sockfd));
    channel_->setWriteCallback(std::bind(&Connector::handleWrite, this));
    channel_->setErrorCallback(std::bind(&Connector::handleError, this));
    channel_->enableWriting();
}

int Connector::removeAndResetChannel()
{
    channel_->disableAll();
    loop_->removeChannel(get_pointer(channel_));
    int sockfd = channel_->fd();
    loop_->queueInLoop(std::bind(&Connector::resetChannel, this));
    return sockfd;
}

void Connector::resetChannel()
{
    channel_.reset();
}

void Connector::handleWrite()
{
    if(state_ == kConnecting)
    {
        int sockfd = removeAndResetChannel();
        int err = sockets::getSocketError(sockfd);
        if(err)
        {
            retry(sockfd);
        }
        else if(sockets::isSelfConnect(sockfd))
        {
            retry(sockfd);
        }
        else
        {
            setState(kConnected);
            if(connect_)
            {
                newConnectionCallback_(sockfd);
            }
            else
            {
                sockets::close(sockfd);
            }
        }
    }
    else
    {
         // LOG
    }
}

void Connector::handleError()
{
    assert(state_ == kConnecting);

    int sockfd = removeAndResetChannel();
    int err = sockets::getSocketError(sockfd);
    // LOG
    retry(sockfd);
}

void Connector::retry(int sockfd)
{
    sockets::close(sockfd);
    setState(kDisconnected);
    if(connect_)
    {
        timerId_ = loop_->runAfter(retryDelayMs_ / 1000.0, std::bind(&Connector::startInLoop, this));
        retryDelayMs_ = (2 * retryDelayMs_ > kMaxRetryMs)? kMaxRetryMs : 2 * retryDelayMs_;
    }
    else
    {
        // LOG
    }
}
