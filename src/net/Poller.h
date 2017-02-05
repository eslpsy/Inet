#ifndef __INET_POLLER_H
#define __INET_POLLER_H
#include <map>
#include <vector>
#include "../base/Timestamp.h"
#include "../base/Noncopyable.h"
#include "EventLoop.h"

struct pollfd;
namespace inet
{
    class Channel;

    class Poller : Noncopyable
    {
        public:
            typedef std::vector<Channel*> ChannelList;

            Poller(EventLoop* loop);
            
            ~Poller();

            Timestamp poll(int timeoutMs, ChannelList* activeChannels);

            void updateChannel(Channel* channel);

            void assertInLoopThread()
            {
                ownerLoop_->assertInLoopThread();
            }

        private:
            typedef std::vector<struct pollfd> PollFdList;
            typedef std::map<int, Channel*> ChannelMap;

            void fillActiveChannels(int numEvents, ChannelList* activeChannels) const;

            EventLoop* ownerLoop_;
            PollFdList pollfds_;
            ChannelMap channels_;
    };
}

#endif
