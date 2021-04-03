#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <poll.h>
#include <iostream>
#include <vector>
class tcp_server{
public:
    tcp_server(uint port, int OPEN_MAX):
        port_(port), max(OPEN_MAX)
    {
    }
    void start()
    {
      server_start();
    }
    tcp_server(const tcp_server& qwe)=delete;
    ~tcp_server(){
        close(listenfd);
    };
private:
      uint port_;
      int timeout = -1;
      int max;
      int i;
      int listenfd;
      int error;
      void server_start(){
          struct sockaddr_in servaddr;
          struct pollfd clients[max];

          if ((listenfd = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
              throw std::runtime_error("socket create error");
          } else {
                  std::cerr<<"Create listen socket "<<listenfd<<'\n';
          }
          memset(&servaddr, 0, sizeof(servaddr));
          servaddr.sin_family = AF_INET;
          servaddr.sin_addr.s_addr = inet_addr("127.0.0.1");
          servaddr.sin_port = htons(port_);
          if ((error = bind(listenfd, (struct sockaddr *)&servaddr, sizeof(servaddr))) < 0) {
              std::cerr<<error<<'\n';
              throw std::runtime_error("error on bind");
          }
          if (listen(listenfd, 10) < 0) {
              throw std::runtime_error("error on listen");
          }
          clients[0].fd = listenfd;
          clients[0].events = POLLIN;
          for (i = 1; i < max; i++) {
                  clients[i].fd = -1;     // -1 indicates available entry
          }

          poller(listenfd, clients);
      }
      void poller(int listenfd, struct pollfd *clients){
          int nready, connfd, sockfd, maxi;
          socklen_t clilen;
          struct sockaddr_in cliaddr;
          ssize_t n;
          std::vector<char>buffer;
          buffer.resize(1024);
          maxi = 0;
          while (1) {
                  nready = poll(clients, maxi + 1, timeout);
                  if (nready <= 0) {
                          continue;
                  }
                  if (clients[0].revents & POLLIN) {
                          clilen = sizeof(cliaddr);
                          if ((connfd = accept(listenfd, (struct sockaddr *)&cliaddr, &clilen)) < 0) {
                                  std::cerr<<"Error: accept\n";
                                  return;
                          }
                          std::cerr<<"Accept socket with id"<<connfd<<"\n addres "<<inet_ntoa(cliaddr.sin_addr)<<"\n port "<<ntohs(cliaddr.sin_port)<<'\n';
                          for (i = 0; i < max; i++) {
                                  if (clients[i].fd < 0) {
                                          clients[i].fd = connfd;
                                          break;
                                  }
                          }
                          if (i == max) {
                                  std::cerr<<"Error: too many clients\n";
                                  close(connfd);
                          }
                          clients[i].events = POLLIN;
                          if (i > maxi) {
                                  maxi = i;
                          }
                          if (--nready <= 0) {
                                  continue;
                          }
                  }

                  for (i = 1; i <= maxi; i++) {
                          if ((sockfd = clients[i].fd) < 0) {
                                  continue;
                          }
                          if (clients[i].revents & (POLLIN | POLLERR)) {
                                  n = read(sockfd, &buffer, 1024);
                                  if (n < 0) {
                                          std::cerr<<"Error: read from socket "<<sockfd<<'\n';
                                          close(sockfd);
                                          clients[i].fd = -1;
                                  } else if (n == 0) {    // connection closed by client
                                          std::cerr<<"Close socket "<<sockfd<<'\n';
                                          close(sockfd);
                                          clients[i].fd = -1;
                                  } else {
                                          std::cerr<<"Read "<<n<<" bytes from socket "<<sockfd<<'\n';
                                          write(sockfd, &buffer, n);
                                          std::cerr<<"write "<<n<<" bytes\n";
                                  }
                                  if (--nready <= 0) {
                                          break;
                                  }
                          }
                  }

          }
      }
};
