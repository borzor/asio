#include <sys/ioctl.h>
#include "reactor.hpp"

class client
{
private:
    ushort proxy_port;
    ushort port_2_connect;
    uint error;
    uint socket_id;
    uint method;
    std::string IP;
    size_t message_size;
    struct sockaddr_in addr;//local host
    std::vector<char>buffer;
public:
    client(ushort port_, ushort port_2, uint method, std::string IP, std::size_t message_size);
    client(client&& mv);
    ~client();
    void socket_create();
    uint get_socket_id() const;
    uint get_method() const;
    size_t dissconect();
    void socks5_handshake_write(reactor &reactor);
    void socks5_handshake_read(reactor &reactor);
    void socks5_request(reactor &reactor);
    void socks5_request_read(reactor &reactor);
    void do_write(reactor &reactor);
    void do_read(reactor &reactor);
    friend class reactor;
};
