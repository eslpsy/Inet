#ifndef __INET_CHANNEL_H
#define __INET_CHANNEL_H

#include <functional>
#include "../base/Noncopyable.h"

namespace inet
{
    class EventLoop;

    class Channel : Noncopyable
    {
        public:
            typedef std::function<void ()> EventCallback;

            Channel(EventLoop* loop, int fd);

            void handleEvent();

            void setReadCallback(const EventCallback& cb)
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

            EventLoop* ownerLoop()
            {
                return loop_;
            }

        private:
            void update();

            static const int kNoEvent;
            static const int kReadEvent;
            static const int kWriteEvent;

            EventCallback readCallback_;
            EventCallback writeCallback_;
            EventCallback errorCallback_;

            EventLoop* loop_;
            const int fd_;
            int events_;
            int revents_;
            int index_;
    };
}

#endif
