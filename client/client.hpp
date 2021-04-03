#include <iostream>
#include <poll.h>
#include <sys/socket.h>
#include <sys/ioctl.h>
#include <cstring>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <vector>
#include <unistd.h>
#include <tuple>
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
    std::vector<unsigned char>buffer_, buffer_2;
    struct sockaddr_in addr;
    int curret_phase=0;
public:
    client(ushort port_, ushort port_2, uint method, std::string IP, std::size_t message_size);
    client(client&& mv);
    ~client();
    void socket_create();
    void socks5_handshake_write();
    void socks5_handshake_read();
    void socks5_request();
    void socks5_request_read();
    int get_socket_id();
    int get_current_phase();
    void write_(uint socket_, const void *message, uint size);
    void read_(uint socket_, void *message, uint size);
    void do_write();
    void do_read();
};
class poll_wrapper
{
public:
    std::tuple<int,int>wait_for_event(std::vector<pollfd> &fds, nfds_t nfds);
};
