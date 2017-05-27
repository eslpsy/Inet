#ifndef __INET_TCPCONNECTION_H
#define __INET_TCPCONNECTION_H
#include <boost/scoped_ptr.hpp>
#include <boost/any.hpp>
#include <memory>
#include "../base/Noncopyable.h"
#include "InetAddress.h"
#include "Callbacks.h"
#include "Buffer.h"

namespace inet
{
    class Channel;
    class EventLoop;
    class Socket;

    class TcpConnection : Noncopyable, public std::enable_shared_from_this<TcpConnection>
    {
        public:
            TcpConnection(EventLoop* loop,
                          const std::string& name,
                          int sockfd,
                          const InetAddress& localAddr,
                          const InetAddress& peerAddr);
            
            ~TcpConnection();

            EventLoop* getLoop() const
            {
                return loop_;
            }

            const std::string& name() const
            {
                return name_;
            }

            const InetAddress& localAddress() const
            {
                return localAddr_;
            }

            const InetAddress& peerAddress() const
            {
                return peerAddr_;
            }

            bool connected() const
            {
                return state_ == kConnected;
            }

            void send(const Buffer& message);

            void setConnectionCallback(const ConnectionCallback& cb)
            {
                connectionCallback_ = cb;
            }

            void setMessageCallback(const MessageCallback& cb)
            {
                messageCallback_ = cb;
            }

            // used by server, not user
            void setCloseCallback(const CloseCallback& cb)
            {
                closeCallback_ = cb;
            }

            void setWriteCompleteCallback(const WriteCompleteCallback& cb)
            {
                writeCompleteCallback_ = cb;
            }

            void setContext(const boost::any& context)
            {
                context_ = context;
            }

            boost::any* getMutableContext()
            {
                return &context_;
            }

            const boost::any& getContext()
            {
                return context_;
            }

            void send(const std::string& message);

            void shutdown();

            void setTcpNoDelay(bool on);

            void setTcpKeepAlive(bool on);

            void connectEstablished();

            void connectDestroyed();

        private:
            enum StateE {kConnecting, kConnected, kDisconnecting, kDisconnected, };

            void setState(StateE s)
            {
                state_ = s;
            }

            void handleRead(Timestamp time);
            void handleWrite();
            void handleClose();
            void handleError();
            void bufferSendInLoop(const Buffer& buf);
            void sendInLoop(const std::string& message);
            void shutdownInLoop();

            EventLoop* loop_;
            std::string name_;
            StateE state_;
            boost::scoped_ptr<Socket> socket_;
            boost::scoped_ptr<Channel> channel_;
            InetAddress localAddr_;
            InetAddress peerAddr_;
            ConnectionCallback connectionCallback_;
            MessageCallback messageCallback_;
            CloseCallback closeCallback_;
            WriteCompleteCallback writeCompleteCallback_;
            Buffer inputBuffer_;
            Buffer outputBuffer_;
            boost::any context_;
    };

    typedef std::shared_ptr<TcpConnection> TcpConnectionPtr;
}

#endif
