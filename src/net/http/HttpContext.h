#ifndef __INET_HTTP_CONTEXT_H
#define __INET_HTTP_CONTEXT_H

#include "HttpRequest.h"
#include <utility>

namespace inet
{
    class Buffer;

    class HttpContext
    {
        public:
            enum HttpRequestParseState
            {
                kExpectRequestLine,
                kExpectHeaders,
                kExpectBody,
                kGotAll
            };

            HttpContext() : state_(kExpectRequestLine)
            {
            }

            bool parseRequest(Buffer* buf, Timestamp receiveTime);

            bool gotAll() const
            {
                return state_ == kGotAll;
            }

            void reset()
            {
                state_ = kExpectRequestLine;
                HttpRequest temp;
                using std::swap;
                swap(request_, temp);
            }
            
            const HttpRequest& request() const
            {
                return request_;
            }

            HttpRequest& request()
            {
                return request_;
            }

        private:
            bool processRequestLine(const char* begin, const char* end);

            HttpRequest request_;
            HttpRequestParseState state_;
    };
}

#endif
