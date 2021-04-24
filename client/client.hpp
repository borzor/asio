#include <iostream>
#include <poll.h>
#include <sys/socket.h>
#include <sys/ioctl.h>
#include <cstring>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <vector>
#include <unistd.h>
#include <span>
#include <functional>
#include <queue>

class client
{
private:
    ushort proxy_port;
    ushort port_2_connect;
    uint error;
    uint socket_id;
    int timeout = 1000;
    uint method;
    std::string IP;
    struct sockaddr_in addr;//local host
    int curret_phase=0;
    uint message_size;
    std::queue<std::function<void(size_t)>>read_queue;
    std::queue<std::function<void(size_t)>>write_queue;
public:
    client(ushort port_, ushort port_2, uint method, std::string IP, std::size_t message_size);
    client(client&& mv);
    ~client();
    enum type { connect, awrites, areads, awrite, aread, hw, hr, reqw, reqr, dwrite, dread, disc };
    void socket_create();
    size_t async_write_some(std::span<char> buf, size_t size);
    size_t async_read_some(std::span<char> buf, size_t size);
    size_t async_write(std::span<char> buf, size_t size);
    size_t async_read(std::span<char> buf,size_t size);
    void async_connect();
    //void poll_wrapper(short event,  size_t (client::*callback)(std::span<char>, size_t), std::span<char> buf, size_t size);
    size_t getsockopt_(std::span<char> buf, size_t size);
    uint get_socket_id() const;
    uint get_method() const;
    ushort get_connect_port() const;
    const sockaddr_in *get_addr() const;
    uint get_msize() const;
    void add_to_queue(enum type);
    size_t dissconect();
};
