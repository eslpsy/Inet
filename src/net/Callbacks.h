#ifndef __INET_CALLBACKS_H
#define __INET_CALLBACKS_H

#include <functional>

namespace inet
{
    typedef std::function<void ()> TimerCallback;
}

#endif
