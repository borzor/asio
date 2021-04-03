#include "client.hpp"

client::client(ushort port_, ushort port_2, uint method, std::string IP, std::size_t message_size)
    :proxy_port(port_),port_2_connect(port_2), method(method), IP(IP), buffer_(2048), buffer_2(message_size)
    {}
client::client(client&& mv)
    :proxy_port(mv.proxy_port),port_2_connect(mv.port_2_connect),method(mv.method),IP(mv.IP), buffer_(mv.buffer_), buffer_2(mv.buffer_2)
    {}
client::~client()
{
    close(socket_id);
    curret_phase = 0;
}
void client::write_(uint socket_, const void * message,uint size){
    if((error = write(socket_, message, size))<0){
        std::cerr<<"error on write: "<<error;
        return;
    }
}
void client::read_(uint socket_, void * message, uint size){
    if((error = read(socket_, message, size))<0){
        std::cerr<<"error on read: "<<error;
        return;
    }
}
int client::get_socket_id(){
    return socket_id;
}
int client::get_current_phase(){
    return curret_phase;
}
void client::socket_create(){
    int on = 1;
    if((socket_id = socket(PF_INET, SOCK_STREAM, 0))<0){
        std::cerr<<"error on socket with id: "<<socket_id<<'\n';
        return;
    }
    if((error = (setsockopt(socket_id, SOL_SOCKET,  SO_REUSEADDR,(char *)&on, sizeof(on))))<0){
        std::cerr<<"setsockport with error;\n";
        shutdown(socket_id,2);
        return;
    }
    if((error =(ioctl(socket_id, FIONBIO, (char *)&on)))<0){
        std::cerr<<"ioctl with error;\n";
        shutdown(socket_id,2);
        return;
    }
    memset(&addr, 0, sizeof(addr));
    addr.sin_family = PF_INET;
    addr.sin_addr.s_addr = inet_addr(IP.c_str());
    addr.sin_port = htons(proxy_port);
    if((error = bind(socket_id,(struct sockaddr *)&addr, sizeof(addr)))<0){
        std::cerr<<"error in bind  witd socket_id"<<socket_id<<"\nand error:"<<error;
        return;
    }
    if((error = connect(socket_id, (struct sockaddr *)&addr, sizeof (addr)))<0){
        std::cerr<<"error on connect: "<<error;
        return;
    }
    std::cerr<<"socket "<<socket_id<<" created\n";
}
void client::socks5_handshake_write(){
    buffer_[0] = 0x05;
    buffer_[1] = 0x01;
    buffer_[2] = method;
    write_(socket_id, &buffer_[0], 3);
}
void client::socks5_handshake_read(){
    read_(socket_id, &buffer_[0], 2);
    if(buffer_[0]!=0x05 || buffer_[1]!=method){
        std::cerr<<"error on first answer from server\n";
        return;
    }
    curret_phase = 1;
}
void client::socks5_request(){
    buffer_[0]=0x05; buffer_[1]=0x01; buffer_[2]=0x00; buffer_[3]=0x01;
    uint16_t second_port = htons(port_2_connect);
    memcpy(&buffer_[4], &addr.sin_addr.s_addr, 4);
    memcpy(&buffer_[8],&second_port, 2);
    write_(socket_id, &buffer_[0], 10);
}
void client::socks5_request_read(){
    read_(socket_id, &buffer_[0], 10);
    if(buffer_[1]!=0x00){
        std::cerr<<"error on first answer from server\n";
        return;
    }
    curret_phase = 2;
}
void client::do_write(){
    memset(&buffer_2[0], 1, buffer_2.size());
    write_(socket_id, &buffer_2[0], buffer_2.size());
}
void client::do_read(){
    read_(socket_id, &buffer_2[0], buffer_2.size());
}
std::tuple<int,int> poll_wrapper::wait_for_event(std::vector<pollfd>&fds, nfds_t nfds){
    std::cerr<<nfds<<'\n'<<fds[0].fd<<'\n'<<fds[0].events<<'\n';
    int i = poll(&fds[0], nfds, -1);
    std::cerr<<"after poll\n";
    return std::make_tuple(fds[i].revents, i);
}




















