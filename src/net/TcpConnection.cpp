#include <functional>
#include <unistd.h>
#include "TcpConnection.h"
#include "Channel.h"
#include "EventLoop.h"
#include "Socket.h"

using namespace inet;

TcpConnection::TcpConnection(EventLoop* loop,
                             const std::string& name,
                             int sockfd,
                             const InetAddress& localAddr,
                             const InetAddress& peerAddr)
 : loop_(loop),
   name_(name),
   state_(kConnecting),
   socket_(new Socket(sockfd)),
   channel_(new Channel(loop, sockfd)),
   localAddr_(localAddr),
   peerAddr_(peerAddr)
{
    channel_->setReadCallback(std::bind(&TcpConnection::handleRead, this, std::placeholders::_1));
    channel_->setWriteCallback(std::bind(&TcpConnection::handleWrite, this));
    channel_->setCloseCallback(std::bind(&TcpConnection::handleClose, this));
    channel_->setErrorCallback(std::bind(&TcpConnection::handleError, this));
}

TcpConnection::~TcpConnection()
{
}

void TcpConnection::connectEstablished()
{
    loop_->assertInLoopThread();
    assert(state_ == kConnecting);
    setState(kConnected);
    channel_->enableReading();

    connectionCallback_(shared_from_this());
}

void TcpConnection::connectDestroyed()
{
    loop_->assertInLoopThread();
    assert(state_ == kConnected || state_ == kDisconnecting);
    setState(kDisconnected);
    channel_->disableAll();
    connectionCallback_(shared_from_this());
    loop_->removeChannel(get_pointer(channel_));
}

void TcpConnection::handleRead(Timestamp time)
{
    int saveErrno = 0;
    ssize_t n = inputBuffer_.readFd(channel_->fd(), &saveErrno);
    if(n > 0)
        messageCallback_(shared_from_this(), &inputBuffer_, time);
    else if(n == 0)
        handleClose();
    else
    {
        errno = saveErrno;
        handleError();
    }
}

void TcpConnection::handleClose()
{
    loop_->assertInLoopThread();
    assert(state_ == kConnected || state_ == kDisconnecting);
    channel_->disableAll();
    closeCallback_(shared_from_this());
}

void TcpConnection::handleError()
{
    // Fix me
}

void TcpConnection::handleWrite()
{
    loop_->assertInLoopThread();
    if(channel_->isWriting())
    {
        ssize_t n = ::write(channel_->fd(), outputBuffer_.peek(), outputBuffer_.readableBytes());
        if(n > 0)
        {
            outputBuffer_.retrieve(n);
            if(outputBuffer_.readableBytes() == 0)
            {
                channel_->disableWriting();
                if(writeCompleteCallback_)
                    loop_->runInLoop(std::bind(writeCompleteCallback_, shared_from_this()));
                if(state_ == kDisconnecting)
                    shutdownInLoop();
            }
            else
            {
                // LOG
            }
        }
        else
        {
            // LOG
        }

    }
    else
    {
        // LOG
    }
}

void TcpConnection::shutdown()
{
    setState(kDisconnecting);
    loop_->runInLoop(std::bind(&TcpConnection::shutdownInLoop, this));
}

void TcpConnection::shutdownInLoop()
{
    loop_->assertInLoopThread();
    if(!channel_->isWriting())
    {
        socket_->shutdownWrite();
    }
}

void TcpConnection::send(const Buffer& message)
{
    if(state_ == kConnected)
    {
        if(loop_->isInLoopThread())
            bufferSendInLoop(message);
        else
            loop_->runInLoop(std::bind(&TcpConnection::bufferSendInLoop, this, message));
    }
}

void TcpConnection::send(const std::string& message)
{
    if(state_ == kConnected)
    {
        if(loop_->isInLoopThread())
            sendInLoop(message);
        else
            loop_->runInLoop(std::bind(&TcpConnection::sendInLoop, this, message));
    }
}

void TcpConnection::bufferSendInLoop(const Buffer& buf)
{
    loop_->assertInLoopThread();
    ssize_t numWrite = 0;
    if(!channel_->isWriting() && outputBuffer_.readableBytes() == 0)
    {
        numWrite = ::write(socket_->fd(), buf.peek(), buf.readableBytes());
        if(numWrite >= 0)
        {
            if(numWrite < buf.readableBytes())
            {
                //LOG
            }
            else if(writeCompleteCallback_)
            {
                writeCompleteCallback_(shared_from_this());
            }
        }
        else
        {
            numWrite = 0;
            if(errno != EWOULDBLOCK)
                abort();
        }
    }

    assert(numWrite >= 0);
    if(numWrite < buf.readableBytes())
    {
        outputBuffer_.append(buf.peek() + numWrite, buf.readableBytes() - numWrite);
        if(!channel_->isWriting())
        {
            channel_->enableWriting();
        }
    }
}

void TcpConnection::sendInLoop(const std::string& message)
{
    loop_->assertInLoopThread();
    ssize_t numWrite = 0;
    if(!channel_->isWriting() && outputBuffer_.readableBytes() == 0)
    {
        numWrite = ::write(socket_->fd(), message.data(), message.size());
        if(numWrite >= 0)
        {
            if(numWrite < message.size())
            {
                //LOG
            }
            else if(writeCompleteCallback_)
                writeCompleteCallback_(shared_from_this());
        }
        else
        {
            numWrite = 0;
            // LOG
            if(errno != EWOULDBLOCK)
                abort();
        }
    }

    assert(numWrite >= 0);
    if(numWrite < message.size())
    {
        outputBuffer_.append(message.data() + numWrite, message.size() - numWrite);
        if(!channel_->isWriting())
            channel_->enableWriting();
    }
}

void TcpConnection::setTcpNoDelay(bool on)
{
    socket_->setTcpNoDelay(on);
}

void TcpConnection::setTcpKeepAlive(bool on)
{
    socket_->setTcpKeepAlive(on);
}
