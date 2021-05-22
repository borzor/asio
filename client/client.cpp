#include "client.hpp"
#include <memory>

client::client(ushort port_, ushort port_2, uint method, std::string IP, std::size_t message_size)
    :proxy_port(port_),port_2_connect(port_2), method(method), IP(IP), message_size(message_size)
    {
    memset(&addr, 0, sizeof(addr));
    inet_pton(AF_INET, IP.c_str(), &addr.sin_addr.s_addr);
    addr.sin_port = htons(proxy_port);
    addr.sin_family = AF_INET;
}
client::client(client&& mv)
    :socket_id(mv.socket_id),proxy_port(mv.proxy_port),port_2_connect(mv.port_2_connect), method(mv.method), IP(mv.IP), message_size(mv.message_size), addr(mv.addr)
    {}
client::~client()
{
    if(socket_id != -1){
        close(socket_id);
    }
}

void client::socket_create(){
    int on = 1;
    if((socket_id = socket(PF_INET, SOCK_STREAM, 0))<0){
        std::cerr<<"error on socket with id: "<<socket_id<<'\n';
        return;
    }
    if((error = (setsockopt(socket_id, SOL_SOCKET,  SO_REUSEADDR,(char *)&on, sizeof(on))))<0){
        std::cerr<<"setsockport with error;\n";
        close(socket_id);
        return;
    }
    if((error =(ioctl(socket_id, FIONBIO, (char *)&on)))<0){
        std::cerr<<"ioctl with error;\n";
        close(socket_id);
        return;
    }
    std::cerr<<"socket "<<socket_id<<" created\n";
}

uint client::get_socket_id() const{
    return socket_id;
}

size_t client::dissconect(){
    return close(socket_id);
}
void client::socks5_handshake_write(reactor &reactor){
    buffer = { 0x05, 0x01, static_cast<char>(method) };
    char a[20];
    inet_ntop(AF_INET, &addr.sin_addr, a, 20);

    connect(socket_id, (const sockaddr*)&addr, sizeof(addr));
    reactor.async_write(reactor, socket_id, std::ref(buffer), [&](size_t){
    socks5_handshake_read(reactor);});
}

void client::socks5_handshake_read(reactor &reactor){
    reactor.async_read(reactor, socket_id, std::ref(buffer), [&](size_t){
        if(buffer[0]!=0x05 || buffer[1]!=method){
            std::cerr<<"error on first answer from server\n";
            return;
        }
        socks5_request(reactor);});
}
void client::socks5_request(reactor &reactor){
    uint16_t second_port = htons(port_2_connect);
    buffer = { 0x05, 0x01, 0x00, 0x01 };

    buffer.resize(10);
    memcpy(&buffer[4], &addr.sin_addr.s_addr, 4);
    memcpy(&buffer[8], &second_port, 2);
    reactor.async_write(reactor, socket_id, std::ref(buffer), [&](size_t){

        socks5_request_read(reactor);});
}

void client::socks5_request_read(reactor &reactor){
    reactor.async_read(reactor, socket_id, std::ref(buffer), [&](size_t){
        if(buffer[1]!=0x00){
            std::cerr<<"error on second answer from server\n";
            return;
        }
        do_write(reactor);});//maybe callback for cheching correct

}
void client::do_write(reactor &reactor){
    buffer.resize(message_size);
    std::fill(buffer.begin(), buffer.end(), 1);
    reactor.async_write(reactor, socket_id, std::ref(buffer), [&](size_t){

        do_read(reactor);});
}
void client::do_read(reactor &reactor){
    std::fill(buffer.begin(), buffer.end(), 0);
    reactor.async_read(reactor, socket_id, std::ref(buffer), [&](size_t){
        int i = dissconect();
        if(i==-1)std::cout<<errno<<'\n';
        socket_create();
        socks5_handshake_write(reactor);
    });

}






























































































