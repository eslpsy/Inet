all: test01 test02 test03 test04 test05 test06 test07 test08 test09 test10 test11

objects = Thread.o EventLoop.o Channel.o Poller.o Timestamp.o Timer.o TimerQueue.o \
		    EventLoopThread.o Acceptor.o Socket.o SocketsOps.o InetAddress.o \
            TcpConnection.o TcpServer.o Buffer.o EventLoopThreadPool.o Connector.o

FLAGS = -g -std=c++11 -pthread

test01 : $(objects) test01.cpp
	clang++ $(objects) $(FLAGS) test01.cpp -o test01

test02 : $(objects) test02.cpp
	clang++ $(objects) $(FLAGS) test02.cpp -o test02

test03 : $(objects) test03.cpp
	clang++ $(objects) $(FLAGS) test03.cpp -o test03

test04 : $(objects) test04.cpp
	clang++ $(objects) $(FLAGS) test04.cpp -o test04

test05 : $(objects) test05.cpp
	clang++ $(objects) $(FLAGS) test05.cpp -o test05

test06 : $(objects) test06.cpp
	clang++ $(objects) $(FLAGS) test06.cpp -o test06

test07 : $(obejcts) test07.cpp
	clang++ $(objects) $(FLAGS) test07.cpp -o test07

test08: $(objects) test08.cpp
	clang++ $(objects) $(FLAGS) test08.cpp -o test08

test09: $(objects) test09.cpp
	clang++ $(objects) $(FLAGS) test09.cpp -o test09

test10: $(objects) test10.cpp
	clang++ $(objects) $(FLAGS) test10.cpp -o test10

test11: $(objects) test11.cpp
	clang++ $(objects) $(FLAGS) test11.cpp -o test11

Thread.o : ../../src/base/Thread.h ../../src/base/Thread.cpp \
	../../src/base/Noncopyable.h ../../src/base/Atomic.h ../../src/base/Thread.cpp
	clang++ $(FLAGS) -c ../../src/base/Thread.cpp

EventLoop.o : ../../src/base/Thread.h ../../src/net/EventLoop.h \
    ../../src/base/Noncopyable.h ../../src/base/Atomic.h ../../src/net/EventLoop.cpp \
    ../../src/base/Mutex.h
	clang++ $(FLAGS) -c ../../src/net/EventLoop.cpp

Channel.o : ../../src/net/Channel.h ../../src/net/Channel.cpp \
	../../src/net/EventLoop.h ../../src/net/Poller.h
	clang++ $(FLAGS) -c ../../src/net/Channel.cpp

Poller.o : ../../src/net/Channel.h ../../src/net/Poller.h \
	../../src/net/EventLoop.h ../../src/net/Poller.cpp
	clang++ $(FLAGS) -c ../../src/net/Poller.cpp

Timestamp.o : ../../src/base/Timestamp.h ../../src/base/Timestamp.cpp
	clang++ $(FLAGS) -c ../../src/base/Timestamp.cpp

Timer.o : ../../src/net/Timer.cpp ../../src/net/Timer.h
	clang++ $(FLAGS) -c ../../src/net/Timer.cpp

TimerQueue.o : ../../src/net/TimerQueue.h ../../src/net/TimerQueue.cpp \
	           ../../src/net/Timer.h ../../src/net/Timer.cpp
	clang++ $(FLAGS) -c ../../src/net/TimerQueue.cpp

EventLoopThread.o : ../../src/net/EventLoopThread.h ../../src/net/EventLoopThread.cpp \
                    ../../src/base/Mutex.h  ../../src/base/Thread.h \
                    ../../src/base/Condition.h ../../src/base/Thread.cpp
	clang++ $(FLAGS) -c ../../src/net/EventLoopThread.cpp

Socket.o: ../../src/net/Socket.h ../../src/net/Socket.cpp
	clang++ $(FLAGS) -c ../../src/net/Socket.cpp

SocketsOps.o: ../../src/net/SocketsOps.h ../../src/net/SocketsOps.cpp
	clang++ $(FLAGS) -c ../../src/net/SocketsOps.cpp

InetAddress.o: ../../src/net/InetAddress.h ../../src/net/InetAddress.cpp
	clang++ $(FLAGS) -c ../../src/net/InetAddress.cpp

Acceptor.o: ../../src/net/Acceptor.h ../../src/net/Acceptor.cpp
	clang++ $(FLAGS) -c ../../src/net/Acceptor.cpp

TcpConnection.o: ../../src/net/TcpConnection.h ../../src/net/TcpConnection.cpp
	clang++ $(FLAGS) -c ../../src/net/TcpConnection.cpp

TcpServer.o: ../../src/net/TcpServer.h ../../src/net/TcpServer.cpp
	clang++ $(FLAGS) -c ../../src/net/TcpServer.cpp

Buffer.o: ../../src/net/Buffer.h ../../src/net/Buffer.cpp
	clang++ $(FLAGS) -c ../../src/net/Buffer.cpp

EventLoopThreadPool.o: ../../src/net/EventLoopThreadPool.h ../../src/net/EventLoopThreadPool.cpp
	clang++ $(FLAGS) -c ../../src/net/EventLoopThreadPool.cpp

Connector.o : ../../src/net/Connector.h ../../src/net/Connector.cpp
	clang++ $(FLAGS) -c ../../src/net/Connector.cpp

clean :
	-rm $(objects) ./test01 ./test02 ./test03 ./test04 ./test05 ./test06 ./test07 ./test08 ./test09 ./test10 ./test11
