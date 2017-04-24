#include "HttpServer.h"
#include "HttpRequest.h"
#include "HttpResponse.h"
#include "HttpContext.h"
#include <functional>

using namespace inet;

namespace inet
{
namespace detail
{
    void defaultHttpCallback(const HttpRequest&, HttpResponse* response)
    {
        response->setStatusCode(HttpResponse::k404NotFound);
        response->setStatusMessage("Not Found");
        response->setCloseConnection(true);
    }
}
}


HttpServer::HttpServer(EventLoop* loop,
                     const InetAddress& listenAddr,
                     TcpServer::Option option)
    : server_(loop, listenAddr, option),
      httpCallback_(detail::defaultHttpCallback)
{
    server_.setConnectionCallback(std::bind(&HttpServer::onConnection, this,
                                            std::placeholders::_1));
    server_.setMessageCallback(std::bind(&HttpServer::onMessage, this,
                                         std::placeholders::_1,
                                         std::placeholders::_2,
                                         std::placeholders::_3));
}

HttpServer::~HttpServer()
{
}

void HttpServer::start()
{
    server_.start();
}

void HttpServer::onConnection(const TcpConnectionPtr& conn)
{
    if(conn->connected())
    {
        conn->setContext(HttpContext());
    }
}

void HttpServer::onMessage(const TcpConnectionPtr& conn,
                          Buffer* buf,
                          Timestamp receiveTime)
{
    HttpContext* context = boost::any_cast<HttpContext>(conn->getMutableContext());

    if(!context->parseRequest(buf, receiveTime))
    {
        conn->send("HTTP/1.1 400 Bad Request\r\n\r\n");
        conn->shutdown();
    }

    if(context->gotAll())
    {
        onRequest(conn, context->request());
        context->reset();
    }
}

void HttpServer::onRequest(const TcpConnectionPtr& conn, const HttpRequest& req)
{
    const std::string& connection = req.getHeader("Connection");
    bool close = connection == "close" || 
        (req.getVersion() == HttpRequest::kHttp10 && connection != "Keep-Alive");
    HttpResponse response(close);
    httpCallback_(req, &response);
    Buffer buf;
    response.appendToBuffer(&buf);
    conn->send(buf);
    if(response.closeConnection())
    {
        conn->shutdown();
    }
}
