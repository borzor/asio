#include <sys/ioctl.h>
#include "reactor.hpp"

class client
{
private:
    ushort proxy_port;
    ushort port_2_connect;
    uint error;
    int socket_id;
    uint method;
    std::string IP;
    size_t message_size;
    struct sockaddr_in addr;
    std::vector<char>buffer;
    uint32_t last_socket;
public:
    client(ushort port_, ushort port_2, uint method, std::string IP, std::size_t message_size);
    client(client&& mv);
    ~client();
    void socket_create();
    int get_socket_id() const;
    void disconnect() const;
    void socks5_handshake_write(reactor &reactor, bool flag);
    void socks5_handshake_read(reactor &reactor);
    void socks5_request(reactor &reactor);
    void socks5_request_read(reactor &reactor);
    void do_write(reactor &reactor);
    void do_read(reactor &reactor);
};
