#  SOCKS5 Server 
Socks5[(RFC)](https://tools.ietf.org/html/rfc1928) multithread asynchronous proxy server with using boost::asio callbacks

#### It supports address types:
* `IPv4`
* `DOMAINNAME`
* `IPv6`

#### Supported authentication methods 
* `NO AUTHENTICATION REQUIRED` 
* `Username/Password Authentication for SOCKS V5`[(RFC)](https://tools.ietf.org/html/rfc1929)

# SOCKS5 Client
Socks5 multithread client based on [Reactor](https://www.adamtornhill.com/Patterns%20in%20C%205,%20REACTOR.pdf) pattern with poll in Synchronous Event Demultiplexer.
Created for server load testing. 

