#include <vector>
#include <map>
#include "../base/Timestamp.h"
#include "../net/EventLoop.h"
#include "../base/Noncopyable.h"

struct epoll_event;

namespace inet
{
    class channel;

    class EPoller : Noncopyable
    {
        public:
            typedef std::vector<Channel*> ChannelList;

            EPoller(EventLoop* loop);

            ~EPoller();

            Timestamp poll(int timeoutMs, ChannelList* activeChannels);

            void updateChannel(Channel* channel);

            void removeChannel(Channel* channel);

            void assertInLoopThread()
            {
                ownerLoop_->assertInLoopThread();
            }


        private:
            typedef std::vector<struct epoll_event> EventList;
            
            typedef std::map<int , Channel*> ChannelMap;
            
            static const int kInitEventListSize = 16;

            void fillActiveChannels(int numEvents, ChannelList* activeChannels);

            void update(int operation, Channel* channel);

            EventLoop* ownerLoop_;
            int epollfd_;
            EventList events_;
            ChannelMap channels_;
    };
}
