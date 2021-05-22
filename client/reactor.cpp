#include "reactor.hpp"

reactor::reactor(std::vector<pollfd> fds)
{
    for(int i = 0; i < fds.size(); i++){
        fds_[fds[i].fd]=fds[i];
    }
}

void reactor::run(){
    for(;;){
        if(poll(&fds_[3], fds_.size(), -1) > 0){
            for(auto &i: fds_){
                if((i.second.revents & POLLOUT)){//WRITE
                    int a = write(i.second.fd, &queues[i.second.fd].write_queue.front().second[0], queues[i.second.fd].write_queue.front().second.size());
                    queues[i.second.fd].write_queue.front().first(a);
                    queues[i.second.fd].write_queue.pop();
                }
                else if(i.second.revents & POLLIN){//read
                    int a = read(i.second.fd, &queues[i.second.fd].read_queue.front().second[0], queues[i.second.fd].read_queue.front().second.size());
                    queues[i.second.fd].read_queue.front().first(a);
                    queues[i.second.fd].read_queue.pop();
                }
                else{//ERROR CODES
                    int a = getsockopt(i.second.fd, SOL_SOCKET, SO_ERROR, &buff, (uint*)buff.size());
                    std::cout<<a<<' '<<errno<<'\n';
                }
            }
        }
    }
}
void reactor::async_read(reactor &reactor, uint socket_id, std::vector<char> &buffer, std::function<void(size_t)> handler){
    fds_[socket_id].events = POLLIN;
    fds_[socket_id].revents = 0;
    reactor.queues[socket_id].read_queue.push(std::pair(handler, std::ref(buffer)));
}
void reactor::async_write(reactor &reactor, uint socket_id, std::vector<char> &buffer, std::function<void(size_t)> handler){
    fds_[socket_id].events = POLLOUT;
    fds_[socket_id].revents = 0;
    reactor.queues[socket_id].write_queue.push(std::pair(handler, std::ref(buffer)));
}

