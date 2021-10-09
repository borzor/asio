#include "client.hpp"
#include <thread>
#include <cmath>


void test_(std::vector<client>& client_, std::size_t time){
    std::vector<pollfd> fds(client_.size());
    for(auto i = 0; i < client_.size(); i++){
        client_[i].socket_create();
        fds[i].fd = client_[i].get_socket_id();
    }
    reactor reactor(fds);
    for(auto i = 0; i < fds.size(); i++){
        client_[i].socks5_handshake_write(reactor, false);
    }
    reactor.run(time);
}


