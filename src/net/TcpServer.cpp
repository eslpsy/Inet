#include <functional>
#include <stdio.h>
#include "Acceptor.h"
#include "EventLoopThreadPool.h"
#include "EventLoop.h"
#include "SocketsOps.h"
#include "TcpServer.h"

using namespace inet;

TcpServer::TcpServer(EventLoop* loop, const InetAddress& listenAddr)
 : loop_(loop),
   name_(listenAddr.toHostPort()),
   acceptor_(new Acceptor(loop, listenAddr)),
   threadPool_(new EventLoopThreadPool(loop_)),
   started_(false),
   nextConnId_(1)
{
    acceptor_->setNewConnectionCallback(std::bind(&TcpServer::newConnection, this, 
                                                std::placeholders::_1, std::placeholders::_2));
}

TcpServer::~TcpServer()
{
}

void TcpServer::start()
{
    if(!started_)
    {
        started_ = true;
        threadPool_->start();
    }

    if(!acceptor_->listening())
    {
        loop_->runInLoop(std::bind(&Acceptor::listen, get_pointer(acceptor_)));
    }
}

void TcpServer::setThreadNum(int numThreads)
{
    assert(numThreads >= 0);
    threadPool_->setThreadNum(numThreads);
}

void TcpServer::newConnection(int sockfd, const InetAddress& peerAddr)
{
    loop_->assertInLoopThread();
    char buf[32];
    snprintf(buf, sizeof(buf), "#%d", nextConnId_);
    ++nextConnId_;
    std::string connName = name_ + buf;
    InetAddress localAddr(sockets::getLocalAddress(sockfd));
    EventLoop* newLoop_ = threadPool_->getNextLoop();
    TcpConnectionPtr conn(new TcpConnection(newLoop_, connName, sockfd, localAddr, peerAddr));
    connections_[connName] = conn;
    conn->setConnectionCallback(connectionCallback_);
    conn->setMessageCallback(messageCallback_);
    conn->setCloseCallback(std::bind(&TcpServer::removeConnection, this,
                                                                   std::placeholders::_1));
    newLoop_->runInLoop(std::bind(&TcpConnection::connectEstablished, conn));
}

void TcpServer::removeConnection(const TcpConnectionPtr& conn)
{
    loop_->runInLoop(std::bind(&TcpServer::removeConnectionInLoop, this, conn));
}

void TcpServer::removeConnectionInLoop(const TcpConnectionPtr& conn)
{
    loop_->assertInLoopThread();
    size_t n = connections_.erase(conn->name());
    assert(n == 1);
    EventLoop* theLoop = conn->getLoop();
    theLoop->queueInLoop(std::bind(&TcpConnection::connectDestroyed, conn));
}
