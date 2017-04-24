#include "EventLoop.h"
#include "Acceptor.h"
#include "InetAddress.h"
#include "SocketsOps.h"

using namespace inet;

Acceptor::Acceptor(EventLoop* loop, const InetAddress& listenAddr, bool reuseAddr)
 : loop_(loop),
   acceptSocket_(sockets::createNonblockOrDie()),
   acceptChannel_(loop, acceptSocket_.fd()),
   listening_(false)
{
    acceptSocket_.setReuseAddr(reuseAddr);
    acceptSocket_.bindAddress(listenAddr);
    acceptChannel_.setReadCallback(std::bind(&Acceptor::handleRead, this));
}

void Acceptor::listen()
{
    loop_->assertInLoopThread();
    listening_ = true;
    acceptSocket_.listen();
    acceptChannel_.enableReading();
}

void Acceptor::handleRead()
{
    loop_->assertInLoopThread();
    InetAddress peerAddr(static_cast<uint16_t>(0));
    int connfd = acceptSocket_.accept(&peerAddr);
    if(connfd >= 0)
    {
        if(newConnectionCallback_)
            newConnectionCallback_(connfd, peerAddr);
        else
            sockets::close(connfd);
    }
}
