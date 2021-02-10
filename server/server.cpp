#include <cstdlib>
#include <iostream>
#include <memory>
#include <utility>
#include <boost/asio.hpp>
#include <string>
#include <boost/endian/conversion.hpp>

using boost::asio::ip::tcp;
class session: public std::enable_shared_from_this<session>
{
public:
    session(tcp::socket socket, boost::asio::io_context &io):
        socket_(std::move(socket)),
        socket_server(io),
        resolver_(io)
    {
        buffer_.resize(1024);
        buffer_2.resize(1024);
    }
      void start()
    {
        first_greetings();
    }

private:
    tcp::socket socket_;
    tcp::socket socket_server;
    std::vector<unsigned char>buffer_;
    std::vector<char>buffer_2;
    std::string hostname;
    std::string port;
    tcp::resolver resolver_;

    void first_greetings()
     {
         auto self(shared_from_this());
         std::cerr<<"first_greetings\n";
         boost::asio::async_read(socket_, boost::asio::buffer(buffer_2, 2),
                                 [this, self](boost::system::error_code ec, std::size_t length)
         {
             if(!ec)
             {
                 std::cerr<<length<<'\n';
                 if (buffer_2[0] != 0x05)
                 {
                     return;
                 }
                 first_greetings_2();
             }
         });
     }
     void first_greetings_2(){
         auto self(shared_from_this());
         std::cerr<<"first_greetings_2\n";
         boost::asio::async_read(socket_, boost::asio::buffer(buffer_, buffer_2[1]),
                                 [this, self](boost::system::error_code ec, std::size_t)
         {
            if(!ec)
            {
                buffer_[0] = 0x05;
                buffer_[1] = 0xFF;
                for(int i = 0; i < buffer_[1]; ++i){
                    if (buffer_[2+i]==0x00)
                        buffer_[1]=0x00;
                }
            }
            first_answering();
         });
     }
     void first_answering(){
         auto self(shared_from_this());
         std::cerr<<"first_answering\n";
         boost::asio::async_write(socket_, boost::asio::buffer(buffer_, 2),
                                  [this, self](boost::system::error_code ec, std::size_t)
         {
             if(buffer_[1] == 0xFF)
                 return;
             if(!ec){
                 read_request();
             }
         });
     }

    void read_request(){
        auto self(shared_from_this());
        std::cerr<<"read_request\n";
        boost::asio::async_read(socket_, boost::asio::buffer(buffer_2, 4),
                                     [this, self](boost::system::error_code ec, std::size_t)
        {
            if(!ec)
            {
                int counter = 0;
                if(buffer_2[0] != 0x05 || buffer_2[1] != 0x01 || buffer_2[2] != 0x00 || buffer_2[3] == 0x04)
                {
                    return;
                }
                if(buffer_2[3]==0x01){
                    counter = 4;
                }
                else if(buffer_2[3]==0x03){
                    boost::asio::async_read(socket_, boost::asio::buffer(&counter,1),
                                            [self](boost::system::error_code, std::size_t){});
                }
                std::cerr<<counter<<'\n';
                second_read_request(counter);
            }
    });
    }
    void second_read_request(int counter){
        auto self(shared_from_this());
        std::cerr<<"second_read_request\n";
        boost::asio::async_read(socket_, boost::asio::buffer(buffer_, counter+2),
                                     [this, self, counter](boost::system::error_code ec, std::size_t)
        {
            if(!ec)
            {
                switch(buffer_2[3])
                {
                case 0x01://IPv4
                    hostname = boost::asio::ip::address_v4(boost::endian::endian_load<uint32_t,4,boost::endian::order::big>(&buffer_[0])).to_string();
                    port = std::to_string(boost::endian::endian_load<uint16_t,2,boost::endian::order::big>(&buffer_[4]));
                    std::cout<<"hostname - "<<hostname<<'\n'<<"port - "<<port<<'\n';
                    break;
                case 0x03://domain name
                    hostname = std::string(buffer_.begin(),buffer_.begin()+counter);
                    port = std::to_string(boost::endian::endian_load<uint16_t,2,boost::endian::order::big>(&buffer_[counter]));
                    std::cout<<"hostname - "<<hostname<<'\n'<<"port - "<<port<<'\n';
                    break;
                case 0x04://IPv6
                    //...
                    break;
                default:
                    //error on addres
                    break;
                }
                resolve();
            }
        });
    }
    void resolve(){
        auto self(shared_from_this());
        std::cerr<<"resolve\n";
        resolver_.async_resolve(tcp::resolver::query({hostname, port}),
            [this, self](const boost::system::error_code& ec, boost::asio::ip::tcp::resolver::results_type result)
            {
                if (!ec)
                {
                    first_connect(result->endpoint());
                }
            });
    }
    void first_connect(tcp::endpoint endpoint_){
        auto self(shared_from_this());
        std::cerr<<"first_connect\n";
        socket_server.async_connect(endpoint_,
                                   [this, self](const boost::system::error_code& ec)
        {
            switch (ec.value()) { // no 0x01: general failure, 0x02: connection not allowed by ruleset, 0x07: command not supported / protocol error
                case boost::system::errc::success:
                    buffer_2[1] = 0x00;
                    break;
                case boost::system::errc::network_down:
                    buffer_2[1] = 0x03;
                    break;
                case boost::system::errc::host_unreachable:
                    buffer_2[1] = 0x04;
                    break;
                case boost::system::errc::connection_aborted:
                    buffer_2[1] = 0x05;
                    break;
                case boost::system::errc::timed_out:
                    buffer_2[1] = 0x06;
                    break;
                case boost::system::errc::address_not_available:
                    buffer_2[1] = 0x08;
                    break;
            }
            second_response();

        });
    }
    void second_response(){
        auto self(shared_from_this());
        std::cerr<<"second_response\n";
        boost::asio::async_write(socket_, boost::asio::buffer(buffer_2),
                                 [this,self](const boost::system::error_code& ec, size_t)
        {
            if (!ec)
            {
                if (buffer_2[1] == 0x00)
                {
                    do_something();
                }
            }
        }
        );
    }

    void do_something(){
        std::cerr<<"do_something\n";
    };
};
class server
{
public:
  server(boost::asio::io_context& io_context, short port)
    : acceptor_(io_context, tcp::endpoint(tcp::v4(), port)),
      io_context(io_context)
  {
    do_accept();
  }

private:
  void do_accept()
  {
    acceptor_.async_accept(
        [this](boost::system::error_code ec, tcp::socket socket)
        {
          if (!ec)
          {
            std::make_shared<session>(std::move(socket), io_context)->start();
          }

          do_accept();
        });
  }

  tcp::acceptor acceptor_;
  boost::asio::io_context &io_context;
};
