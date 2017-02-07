#ifndef __INET_TCPSERVER_H
#define __INET_TCPSERVER_H
#include <map>
#include <string>
#include <boost/scoped_ptr.hpp>
#include "../base/Noncopyable.h"
#include "Callbacks.h"
#include "TcpConnection.h"

namespace inet
{
    class Acceptor;
    class EventLoop;
    class EventLoopThreadPool;

    class TcpServer : Noncopyable
    {
        public:
            TcpServer(EventLoop* loop, const InetAddress& listenAddr);
            ~TcpServer();

            void start();

            void setThreadNum(int numThreads);

            void setConnectionCallback(const ConnectionCallback& cb)
            {
                connectionCallback_ = cb;
            }

            void setMessageCallback(const MessageCallback& cb)
            {
                messageCallback_ = cb;
            }

        private:
            typedef std::map<std::string, TcpConnectionPtr> ConnectionMap;
            
            void newConnection(int sockfd, const InetAddress& peerAddr);

            void removeConnection(const TcpConnectionPtr& conn);

            void removeConnectionInLoop(const TcpConnectionPtr& conn);

            EventLoop* loop_;
            const std::string name_;
            boost::scoped_ptr<Acceptor> acceptor_;
            boost::scoped_ptr<EventLoopThreadPool> threadPool_;
            ConnectionCallback connectionCallback_;
            MessageCallback messageCallback_;
            bool started_;
            int nextConnId_;
            ConnectionMap connections_;
    };
}
#endif
