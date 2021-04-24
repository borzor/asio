#include "reactor.hpp"

reactor::reactor(std::vector<pollfd> fds)
    :fds(fds)
{}

void reactor::run(){
    for(auto &i: fds){
        async_connect(i.fd);
    }
    for(;;){
        if(poll(&fds[0], fds.size(), 0)>0){
            for(auto &i: fds){
                if(i.revents & POLLIN){//read
                    int a = write(i.fd, &write_queue.front().second.buffer, write_queue.front().second.size);
                    write_queue.front().first(a);
                    write_queue.pop();
                }
                if(i.revents & POLLOUT){//write
                    int a = read(i.fd, &read_queue.front().second.buffer, read_queue.front().second.size);
                    read_queue.front().first(a);
                    read_queue.pop();
                }
        }
    }
}
}
void reactor::async_connect(uint socket_id){
    connect(socket_id, (struct sockaddr *)&addr, sizeof (addr));
    std::span<char>buffer;
    size_t size = 0;
    buf buf{buffer, size};
    write_queue.push(std::pair([&](size_t){getsockopt_(socket_id, buffer, size);}, buf));
}
size_t reactor::getsockopt_(uint socket_id, std::span<char> buf, size_t size){
    if((error = getsockopt(socket_id, SOL_SOCKET, SO_ERROR, &buf, (uint*)buf.size())) < 0){// ??????????????????????????????????????
        return 0;}// error on getsockopt?)))))))))))))
    else if(memcmp(&buf, &error, 1)){//??????????
            async_connect();// if error try to connect
        }
    write_queue.push_back([&](size_t){socks5::socks5_handshake_write(*this);});
    return 0;
}



/*
size_t reactor::async_write_some(uint socket_id, std::span<char> buf, size_t size){// size parametr only for poll
    size_t writen_bytes = 0;
    if((writen_bytes = write(socket_id, &buf, size))<0){//can be infinite cycle? / maybe classificate errors / check on connection
        write_queue.push(std::pair([&](size_t){async_write_some(socket_id, buf, size);}), 1);
    }
    else
        return writen_bytes;
}
size_t reactor::async_read_some(uint socket_id, std::span<char> buf, size_t size){
    size_t bytes_read = 0;
    if((bytes_read = read(socket_id, &buf, size))<0){
        read_queue.push_front([&](size_t){async_read_some(socket_id, buf, size);});
    }
    else
        return bytes_read;
}

size_t reactor::async_write(uint socket_id, std::span<char> buf, size_t size){// errors to break cycle? getsockopt?
    size_t writen_bytes = 0;
    for(;;){
        if((writen_bytes += async_write_some(socket_id, buf.subspan(writen_bytes), size-writen_bytes)) == size){
            return writen_bytes;
        }
        else{
            write_queue.push_front([&](size_t){async_write(socket_id, buf.subspan(writen_bytes), size-writen_bytes);});
        }
    }
}

size_t reactor::async_read(uint socket_id, std::span<char> buf, size_t size){
    size_t bytes_read = 0;
    for(;;){
        if((bytes_read += async_read_some(socket_id, buf.subspan(bytes_read), size-bytes_read)) == size){
            return bytes_read;
        }
        else{
            read_queue.push_front([&](size_t){async_read(socket_id, buf.subspan(bytes_read), size-bytes_read);});
        }
    }
}
*/
