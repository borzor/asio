#include "socks5.hpp"

client::client(ushort port_, ushort port_2, uint method, std::string IP, std::size_t message_size)
    :proxy_port(port_),port_2_connect(port_2), method(method), IP(IP), message_size(message_size)
    {}
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

size_t client::async_write_some(std::span<char> buf, size_t size){// size parametr only for poll
    size_t writen_bytes = 0;
    if((writen_bytes = write(socket_id, &buf, size))<0){//can be infinite cycle?
        std::cerr<<"error on write: "<<writen_bytes;
        add_to_queue()
        size_t (client::*callback)(std::span<char>, size_t);
        callback = &client::async_write_some;
        poll_wrapper(POLLOUT, callback, buf, size);
    }
    else
        return writen_bytes;
}
size_t client::async_read_some(std::span<char> buf, size_t size){
    size_t bytes_read = 0;
    if((bytes_read = read(socket_id, &buf, size))<0){
        std::cerr<<"error on async_read_some: "<<bytes_read;
        size_t (client::*callback)(std::span<char>, size_t);
        callback = &client::async_read_some;
        poll_wrapper(POLLIN, callback, buf, size);
    }
    else
        return bytes_read;
}

size_t client::async_write(std::span<char> buf, size_t size){// errors to break cycle?
    size_t writen_bytes = 0;
    for(;;){
        if(((writen_bytes += async_write_some(buf.subspan(writen_bytes), size-writen_bytes)) = size)){
            return writen_bytes;
        }
        else{
            size_t (client::*callback)(std::span<char>, size_t);
            callback = &client::async_write;
            poll_wrapper(POLLIN, callback, buf.subspan(writen_bytes), size-writen_bytes);
        }
    }
}

size_t client::async_read(std::span<char> buf, size_t size){
    size_t bytes_read = 0;
    for(;;){
        if(((bytes_read += async_read_some(buf.subspan(bytes_read), size-bytes_read)) = size)){
            return bytes_read;
        }
        else{
            size_t (client::*callback)(std::span<char>, size_t);
            callback = &client::async_read;
            poll_wrapper(POLLIN, callback, buf.subspan(bytes_read), size-bytes_read);
        }
    }
}

size_t client::getsockopt_(std::span<char> buf, size_t size){
    size_t error = 0;
    if((error = getsockopt(socket_id, SOL_SOCKET, SO_ERROR, &buf, (uint*)size)) < 0){// ??????????????????????????????????????
        return 0;}// error on getsockopt?)))))))))))))
    else if(memcmp(&buf, &error, 1)){//??????????
            async_connect();// if error try to connect
        }
    add_to_queue(hw);
    return 0;
}

void client::async_connect(){
    std::span<char> buf;
    uint size = sizeof(buf);
    connect(socket_id, (struct sockaddr *)&addr, sizeof (addr));
    size_t (client::*callback)(std::span<char>, size_t);
    callback = &client::getsockopt_;
    poll_wrapper(POLLOUT, callback, buf, size);
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
uint client::get_method() const{
    return method;
}
ushort client::get_connect_port() const{
    return port_2_connect;
}
const sockaddr_in *client::get_addr() const{
    return &addr;
}
uint client::get_msize() const{
    return message_size;
}
void client::add_to_queue(type type){
    switch(type)
    {
    case connect:
        write_queue.push([&](size_t){async_connect();});
    case awrites:
        write_queue.push([&]())
    case hw:
        write_queue.push([&](size_t){socks5::socks5_handshake_write(*this);});
    case hr:
        read_queue.push([&](size_t){socks5::socks5_handshake_read(*this);});
    case reqw:
        write_queue.push([&](size_t){socks5::socks5_request(*this);});
    case reqr:
        read_queue.push([&](size_t){socks5::socks5_request_read(*this);});
    case dwrite:
        write_queue.push([&](size_t){socks5::do_write(*this);});
    case dread:
        read_queue.push([&](size_t){socks5::do_read(*this);});
    case disc:
        dissconect();
    }
}
size_t client::dissconect(){
    return shutdown(socket_id, 2);
}































































































