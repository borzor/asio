#include "socks5.hpp"

void socks5::socks5_handshake_write(client &client){
    char a[] = { 0x05, 0x01, static_cast<char>(0x00) };//last method
    client.async_write(a, 3);
    client.add_to_queue(client::type::hr);
}

void socks5::socks5_handshake_read(client &client){
    std::vector<char>buffer(2);
    client.async_read(buffer, 2);
    if(buffer[0]!=0x05 || buffer[1]!=client.get_method()){
        std::cerr<<"error on first answer from server\n";
        return;
    }
    client.add_to_queue(client::type::reqw);
}
void socks5::socks5_request(client &client){
    uint16_t second_port = htons(client.get_connect_port());
    std::vector<char> buffer = { 0x05, 0x01, 0x00, 0x01 };
    memcpy(&buffer[4], &client.get_addr()->sin_addr.s_addr, 4);
    memcpy(&buffer[8],&second_port, 2);
    client.async_write(buffer, 10);
    client.add_to_queue(client::type::reqr);
}

void socks5::socks5_request_read(client &client){
    std::vector<char>buffer(10);
    client.async_read(buffer, 10);
    if(buffer[1]!=0x00){
        std::cerr<<"error on first answer from server\n";
        return;
    }
    client.
    client.add_to_queue(client::type::dwrite);
}
void socks5::do_write(client &client){
    std::vector<char>buf(client.get_msize(), 0);
    client.async_write(buf, client.get_msize());
    client.add_to_queue(client::type::dread);
}
void socks5::do_read(client &client){
    std::span<char>buf;
    client.async_read(buf, client.get_msize());
}


















