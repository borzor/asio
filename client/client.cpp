#include "client.hpp"

client::client(ushort port_, ushort port_2, uint method, std::string IP, std::size_t message_size)
    :proxy_port(port_),port_2_connect(port_2), method(method), IP(IP), message_size(message_size)
    {
    inet_pton(AF_INET, IP.c_str(), &addr.sin_addr);
    addr.sin_port = proxy_port;
}
client::client(client&& mv)
    :socket_id(mv.socket_id){
    mv.socket_id = -1;
    }
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
    std::cout<<addr.sin_port<<'\n';
    std::cout<<port_2_connect<<'\n';
    std::cout<<method<<'\n';
    std::cerr<<"socket "<<socket_id<<" created\n";
}

uint client::get_socket_id() const{
    return socket_id;
}
uint client::get_method() const{
    return method;
}

size_t client::dissconect(){
    return shutdown(socket_id, 2);
}
void client::socks5_handshake_write(reactor &reactor){
    buffer = { 0x05, 0x01, static_cast<char>(method) };
    connect(socket_id, (const sockaddr*)&addr, sizeof(addr));
    reactor.async_write(reactor, socket_id, buffer, 3, [&](size_t c){

        socks5_handshake_read(reactor);});
}

void client::socks5_handshake_read(reactor &reactor){
    std::vector<char>buffer(2);
    reactor.async_read(reactor, socket_id, buffer, 2, [&](size_t){

        if(buffer[0]!=0x05 || buffer[1]!=method){
            std::cerr<<"error on first answer from server\n";
            return;
        }
        socks5_request(reactor);});//maybe callback for cheching correct
}
void client::socks5_request(reactor &reactor){
    uint16_t second_port = htons(port_2_connect);
    std::vector<char> buffer = { 0x05, 0x01, 0x00, 0x01 };
    memcpy(&buffer[4], &addr.sin_addr.s_addr, 4);
    memcpy(&buffer[8], &second_port, 2);
    reactor.async_write(reactor, socket_id, buffer, 10, [&](size_t){

        socks5_request_read(reactor);});
}

void client::socks5_request_read(reactor &reactor){
    std::vector<char>buffer(10);
    reactor.async_read(reactor, socket_id, buffer, 10, [&](size_t){

        if(buffer[1]!=0x00){
            std::cerr<<"error on first answer from server\n";
            return;
        }
        do_write(reactor);});//maybe callback for cheching correct

}
void client::do_write(reactor &reactor){
    std::vector<char>buffer(message_size, 0);
    reactor.async_write(reactor,  socket_id, buffer, message_size, [&](size_t){

        do_read(reactor);});
}
void client::do_read(reactor &reactor){
    std::span<char>buf;
    //client.sdohni_nahyu();
}






























































































