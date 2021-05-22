#include <iostream>
#include <poll.h>
#include <sys/socket.h>
#include <cstring>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <vector>
#include <unistd.h>
#include <functional>
#include <queue>
#include <map>
#include <set>
class reactor
{
private:
    struct queues_{
        std::queue<std::pair<std::function<void(size_t)>, std::vector<char>&>> read_queue;
        std::queue<std::pair<std::function<void(size_t)>, std::vector<char>&>> write_queue;
    };
    std::map<int, queues_> queues;
    std::map<uint32_t, pollfd> fds_;
    std::vector<char>buff;
public:
    reactor(std::vector<pollfd> fds);
    void async_read(reactor &reactor, uint socket_id, std::vector<char> &buffer, std::function<void (size_t)> handler);
    void async_write(reactor &reactor, uint socket_id, std::vector<char> &buffer, std::function<void(size_t)> handler);
    void run();
    friend class client;
};

