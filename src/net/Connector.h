#ifndef __INET_CONNECTOR_H
#define __INET_CONNECTOR_H

#include <functional>
#include <boost/scoped_ptr.hpp>
#include "../base/Noncopyable.h"
#include "InetAddress.h"
#include "TimerId.h"

namespace inet
{
    class Channel;
    class EventLoop;

    class Connector : Noncopyable
    {
        public:
            typedef std::function<void (int sockfd)> NewConnectionCallback;

            Connector(EventLoop* loop, const InetAddress& serverAddr);
            
            ~Connector();

            void setNewConnectionCallback(const NewConnectionCallback& cb)
            {
                newConnectionCallback_ = cb;
            }

            void start();

            void restart();

            const InetAddress& serverAddress() const
            {
                return serverAddr_;
            }

        private:
            enum States {kDisconnected, kConnecting, kConnected};
            static const int kMaxRetryMs;
            static const int kInitRetryMs;

            void setState(States s)
            {
                state_ = s;
            }

            void startInLoop();

            void connect();

            void connecting(int sockfd);

            void handleWrite();

            void handleError();

            void retry(int sockfd);

            int removeAndResetChannel();

            void resetChannel();

            EventLoop* loop_;
            InetAddress serverAddr_;
            bool connect_;
            States state_;
            boost::scoped_ptr<Channel> channel_;
            NewConnectionCallback newConnectionCallback_;
            int retryDelayMs_;
            TimerId timerId_;
    };
    typedef std::shared_ptr<Connector> ConnectorPtr;
}

#endif
