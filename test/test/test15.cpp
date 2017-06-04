#include "../../src/net/EventLoop.h"
#include "../../src/net/InetAddress.h"
#include "../../src/net/ChatRoom/ChatRoom.h"

int main()
{
    EventLoop loop;
    InetAddress addr(10527);
    
    ChatRoom chat_room(&loop, addr);
    chat_room.start();
    loop.loop();
    
    return 0;
}
