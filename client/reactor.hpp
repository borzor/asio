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
    std::queue<std::pair<std::function<void(size_t)>, buf>>read_queue;
    std::queue<std::pair<std::function<void(size_t)>, buf>>write_queue;
    std::vector<pollfd> fds;
public:
    reactor(std::vector<pollfd> fds);
    size_t async_write_some(uint socket_id, std::span<char> buf, size_t size);
    size_t async_read_some(uint socket_id, std::span<char> buf, size_t size);
    size_t async_write(uint socket_id, std::span<char> buf, size_t size);
    size_t async_read(uint socket_id, std::span<char> buf,size_t size);
    void run();
    friend class client;

};

