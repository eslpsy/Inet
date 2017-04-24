#ifndef __INET_HTTP_SERVER_H
#define __INET_HTTP_SERVER_H
#include "../TcpServer.h"
#include "../../base/Noncopyable.h"

namespace inet
{
    class HttpResponse;
    class HttpRequest;

    class HttpServer : Noncopyable
    {
        public:
           typedef std::function<void (const HttpRequest&, 
                                        HttpResponse*)> HttpCallback;

           HttpServer(EventLoop* loop,
                      const InetAddress& listenAddr,
                      TcpServer::Option option = TcpServer::kNoReusePort);

           ~HttpServer();

           EventLoop* getLoop() const
           {
                return server_.getLoop();
           }

           void start();

           void setThreadNum(int num)
           {
                server_.setThreadNum(num);
           }

           void setHttpCallback(const HttpCallback& cb)
           {
                httpCallback_ = cb;
           }

        private:
           void onConnection(const TcpConnectionPtr& conn);

           void onMessage(const TcpConnectionPtr& conn,
                          Buffer* buf,
                          Timestamp receiveTime);

            void onRequest(const TcpConnectionPtr& conn,
                           const HttpRequest& request);
            
            TcpServer server_;
            HttpCallback httpCallback_;
    };
}

#endif
