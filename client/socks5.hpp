#include "client.hpp"

class socks5
{
public:
    static void socks5_handshake_write(client &client);
    static void socks5_handshake_read(client &client);
    static void socks5_request(client &client);
    static void socks5_request_read(client &client);
    static void do_write(client &client);
    static void do_read(client &client);
};
