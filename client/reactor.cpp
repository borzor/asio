#include "reactor.hpp"

reactor::reactor(std::vector<pollfd> fds):fds_(fds)
{
    for(size_t i = 0; i < fds.size(); ++i){
        fds_map[fds[i].fd] = i;
    }
}

void reactor::run(){
    for(;;){
        if(poll(&fds_[0], fds_.size(), -1) > 0){
            for(auto &i: fds_){
                if(i.revents != 0){
                    if((i.revents & POLLOUT)){//WRITE
                        int a = write(i.fd, &queues[i.fd].write_queue.front().second[0], queues[i.fd].write_queue.front().second.size());
                        queues[i.fd].write_queue.front().first(a);
                        queues[i.fd].write_queue.pop();
                    }
                    else if(i.revents & POLLIN){//read
                        int a = read(i.fd, &queues[i.fd].read_queue.front().second[0], queues[i.fd].read_queue.front().second.size());
                        queues[i.fd].read_queue.front().first(a);
                        queues[i.fd].read_queue.pop();
                    }
                }
            }
        }
    }
}
void reactor::async_read(reactor &reactor, const uint socket_id, std::vector<char> &buffer, std::function<void(size_t)> handler){
    fds_[fds_map[socket_id]].events = POLLIN;
    fds_[fds_map[socket_id]].revents = 0;
    reactor.queues[socket_id].read_queue.push(std::pair(handler, std::ref(buffer)));
}
void reactor::async_write(reactor &reactor, const uint socket_id, std::vector<char> &buffer, std::function<void(size_t)> handler){
    fds_[fds_map[socket_id]].events = POLLOUT;
    fds_[fds_map[socket_id]].revents = 0;
    reactor.queues[socket_id].write_queue.push(std::pair(handler, std::ref(buffer)));
}

void reactor::re(const uint32_t socket, const uint32_t last_socket){
    fds_[fds_map[last_socket]].fd = socket;
    uint32_t index = fds_map.find(last_socket)->second;
    fds_map.erase(fds_map.find(last_socket));
    if(socket != last_socket){
        queues[socket] = queues[last_socket];
        queues.erase(queues.find(last_socket));
    }
    fds_map[socket] = index;
    std::cout<<"new\n";
}
