#include <iostream>
#include <poll.h>
#include <sys/socket.h>
#include <cstring>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <vector>
#include <unistd.h>
#include <span>
#include <functional>
#include <queue>
#include <map>


class reactor
{
private:
    struct buf{
        std::span<char> buffer;
        size_t size;
    };
    struct queues_{
        std::queue<std::pair<std::function<void(size_t)>, buf>>read_queue;
        std::queue<std::pair<std::function<void(size_t)>, buf>>write_queue;
    };
    std::map<int, queues_> queues;
    std::vector<pollfd> fds;
    std::span<char>buff;
public:
    reactor(std::vector<pollfd> fds);
    void async_read(reactor &reactor, uint socket_id, std::span<char> buffer, size_t size, std::function<void (size_t)> handler);
    void async_write(reactor &reactor, uint socket_id, std::span<char> buffer, size_t size, std::function<void(size_t)> handler);
    void run();
    friend class client;

    /*size_t async_write_some(uint socket_id, std::span<char> buf, size_t size);
    size_t async_read_some(uint socket_id, std::span<char> buf, size_t size);
    size_t async_write(uint socket_id, std::span<char> buf, size_t size);
    size_t async_read(uint socket_id, std::span<char> buf,size_t size);*/
};

