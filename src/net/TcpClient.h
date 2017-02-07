#ifndef __INET_TCPCLIENT_H
#define __INET_TCPCLIENT_H
#include <memory>
#include "../../src/base/Mutex.h"
#include "../../src/base/Noncopyable.h"
#include "TcpConnection.h"

namespace inet
{
    class Connector;
    typedef std::shared_ptr<Connector> ConnectorPtr;

    class TcpClient : Noncopyable
    {
        public:
            TcpClient(EventLoop* loop, const InetAddress& serverAddr);

            ~TcpClient();

            void connect();

            void disconnect();

            void stop();

            TcpConnectionPtr connection() const
            {
                MutexLockGuard lock(mutex_);
                return connection_;
            }

            bool retry() const;

            void enableRetry()
            {
                retry_ = true;
            }

            void setConnectionCallback(const ConnectionCallback& cb)
            {
                connectionCallback_ = cb;
            }

            void setMessageCallback(const MessageCallback& cb)
            {
                messageCallback_ = cb;
            }

            void setWriteCompleteCallback(const WriteCompleteCallback& cb)
            {
                writeCompleteCallback_ = cb;
            }

        private:
            void newConnection(int sockfd);

            void removeConnection(const TcpConnectionPtr& conn);

            EventLoop* loop_;
            ConnectorPtr connector_;
            ConnectionCallback connectionCallback_;
            MessageCallback messageCallback_;
            WriteCompleteCallback writeCompleteCallback_;
            bool retry_;
            bool connect_;
            int nextConnId_;
            mutable MutexLock mutex_;
            TcpConnectionPtr connection_;
    };
}
#endif
