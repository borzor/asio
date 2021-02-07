#include <cstdlib>
#include <iostream>
#include <memory>
#include <utility>
#include <boost/asio.hpp>
#include <string>

using boost::asio::ip::tcp;
class session: public std::enable_shared_from_this<session>
{
public:
    session(tcp::socket socket):
        socket_(std::move(socket)),
        resolver(socket.get_executor().context())
    {
    }
      void start()
    {
        first_greetings();
    }
private:
    tcp::socket socket_;
    std::vector<unsigned char>buffer_;
    std::vector<char>buffer_2;
    std::string hostname;
    std::string port;
    tcp::resolver resolver;

    void first_greetings()
    {
        auto self(shared_from_this());
        socket_.async_read_some(boost::asio::buffer(buffer_),
                                [this, self](boost::system::error_code ec, std::size_t length)
        {
            if(!ec)
            {
                if (length < 3 || buffer_[0] != 0x05)
                {
                    throw std::invalid_argument("invalid message from client");
                }
                int counter = buffer_[1];
                buffer_[1] = 0xFF;
                for(int i = 0; i < counter; ++i){
                    if (buffer_[2+i]==0x00)
                        buffer_[1]=0x00;
                }
                first_answering();
            }
            else{
                throw std::logic_error("request error");
            }
        });
    }
    void first_answering(){
        auto self(shared_from_this());
        boost::asio::async_write(socket_, boost::asio::buffer(buffer_, 2),
                                 [this, self](boost::system::error_code ec, std::size_t length)
        {
            if(buffer_[1] == 0xFF)
                throw std::invalid_argument("invalid second argument");
            if(!ec){
                read_request();
            }
        });
    }
    void read_request(){
        auto self(shared_from_this());
        socket_.async_read_some(boost::asio::buffer(buffer_2),
                                     [this, self](boost::system::error_code ec, std::size_t length)
        {
            if(!ec)
            {
                if(length < 6 || buffer_[0] != 0x05 || buffer_[1] != 0x01 || buffer_[2] != 0x00 || buffer_[3] == 0x04)
                {
                    throw std::invalid_argument("error on read request");
                }
                switch(buffer_[3])
                {
                case 0x01://IPv4
                    hostname = boost::asio::ip::address_v4(ntohl(*(uint32_t*)(&buffer_2[4]))).to_string();
                    port = std::to_string(ntohs(*((uint16_t*)&buffer_2[8])));
                    break;
                case 0x03://domain name
                    hostname = std::string(&buffer_2[5], buffer_2[4]);
                    port = std::to_string(ntohs(*((uint16_t*)&buffer_2[5+buffer_2[4]])));
                    break;
                case 0x04://IPv6
                    //...
                    break;
                default:
                    //error on addres
                    break;
                }

                first_response();
            }
    });
};
    void first_response(){
        auto self(shared_from_this());
        resolver.async_resolve(tcp::resolver::query({hostname, port}),
            [this, self](const boost::system::error_code& ec, tcp::resolver::iterator it)
            {
                if (!ec)
                {
                    connect(it);
                }
                else
                {
                    //response
                }
            });
    }
    void connect(tcp::resolver::iterator& it){

    }
};
