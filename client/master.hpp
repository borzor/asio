#ifndef MASTER_H
#define MASTER_H
#include "client.hpp"
#include <thread>
#include <cmath>

void test_(std::vector<client>& client_, std::size_t time){
    poll_wrapper poll_;
    std::vector<pollfd> fds(client_.size());
    for(auto i = 0; i < client_.size(); i++){
        client_[i].socket_create();
        fds[i].fd = client_[i].get_socket_id();
        fds[i].events = POLLIN;
        fds[i].revents = 0;
    }
    auto timer = std::chrono::steady_clock::now();
    while(std::chrono::steady_clock::now()<timer+std::chrono::seconds(time)){
        auto [event, pos_num] = poll_.wait_for_event(fds, client_.size());// socks5_handshake_write -> poll -> socks5_request -> poll -> write -> poll
        if(event == POLLOUT){
            switch (client_[pos_num].get_current_phase()) {
                case 0:
                    client_[pos_num].socks5_handshake_write();
                    fds[pos_num].events = POLLIN;// revents?..
                    break;
                case 1:
                    client_[pos_num].socks5_request();
                    fds[pos_num].events = POLLIN;
                    break;
                case 2:
                    client_[pos_num].do_write();
                    fds[pos_num].events = POLLIN;
                    break;
            }
        }
        else if(event == POLLIN){
            switch (client_[pos_num].get_current_phase()) {
                case 0:
                    client_[pos_num].socks5_handshake_read();
                    fds[pos_num].events = POLLOUT;
                    break;
                case 1:
                    client_[pos_num].socks5_request();
                    fds[pos_num].events = POLLOUT;
                    break;
                case 2:
                    client_[pos_num].do_read();
                    close(client_[pos_num].get_socket_id());
                    client_[pos_num].socket_create();
                    fds[pos_num].fd = client_[pos_num].get_socket_id();
                    fds[pos_num].events = POLLIN;
                    break;
            }
        }

    }
}

#endif // MASTER_H
