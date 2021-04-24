#ifndef MASTER_H
#define MASTER_H
#include "client.hpp"
#include <thread>
#include <cmath>

void test_(std::vector<client>& client_, std::size_t time){
    std::vector<pollfd> fds(client_.size());
    for(auto i = 0; i < client_.size(); i++){
        client_[i].socket_create();
        fds[i].fd = client_[i].get_socket_id();
        fds[i].events = POLLOUT;
        fds[i].revents = 0;
        client_[i].add_to_queue(client::type::connect);
    }
    auto timer = std::chrono::steady_clock::now();
    while(std::chrono::steady_clock::now()<timer+std::chrono::seconds(time)){
        poll(&fds[0], fds.size(), -1);
        //if(write)
        //if(read)
    }
}

#endif // MASTER_H

/* socket_create->add connect to queue->
 * poll if(write){
 * map_with_clients[fd].write_queue.front()
 * map_with_clients[fd].write_queue.pop()
 *  * poll if(read){
 * map_with_clients[fd].read_queue.front()
 * map_with_clients[fd].read_queue.pop()
  */
