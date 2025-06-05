#include "src/EventLoop.h"
#include "src/Server.h"

int main(int argc, char const *argv[])
{
    EventLoop *el = new EventLoop();
    Server *server = new Server(el);
    el->loop();

    delete server;
    delete el;
    return 0;
}
