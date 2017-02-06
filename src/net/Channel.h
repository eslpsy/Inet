#ifndef __INET_CHANNEL_H
#define __INET_CHANNEL_H

#include <functional>
#include "../base/Timestamp.h"
#include "../base/Noncopyable.h"

namespace inet
{
    class EventLoop;

    class Channel : Noncopyable
    {
        public:
            typedef std::function<void ()> EventCallback;
            typedef std::function<void (Timestamp)> ReadEventCallback;

            Channel(EventLoop* loop, int fd);

            ~Channel();

            void handleEvent(Timestamp time);

            void setReadCallback(const ReadEventCallback& cb)
            {
                readCallback_ = cb;
            }

            void setWriteCallback(const EventCallback& cb)
            {
                writeCallback_ = cb;
            }

            void setErrorCallback(const EventCallback& cb)
            {
                errorCallback_ = cb;
            }

            void setCloseCallback(const EventCallback& cb)
            {
                closeCallback_ = cb;
            }

            int events() const
            {
                return events_;
            }

            void set_revents(int revents)
            {
                revents_ = revents;
            }

            int fd() const
            {
                return fd_;
            }

            int index() const
            {
                return index_;
            }

            void set_index(int index)
            {
                index_ = index;
            }

            bool isNoEvent() const
            {
                return events_ == kNoEvent;
            }

            void enableReading()
            {
                events_ |= kReadEvent;
                update();
            }

            void disableAll()
            {
                events_ = kNoEvent;
                update();
            }

            EventLoop* ownerLoop()
            {
                return loop_;
            }

        private:
            void update();

            static const int kNoEvent;
            static const int kReadEvent;
            static const int kWriteEvent;

            ReadEventCallback readCallback_;
            EventCallback writeCallback_;
            EventCallback errorCallback_;
            EventCallback closeCallback_;

            EventLoop* loop_;
            const int fd_;
            int events_;
            int revents_;
            int index_;
            bool eventHandling_;
    };
}

#endif
