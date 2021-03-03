#include <cstdlib>
#include <iostream>
#include <memory>
#include <utility>
#include <boost/asio.hpp>
#include <string>
#include <boost/endian/conversion.hpp>
#include <thread>
using boost::asio::ip::tcp;
class session: public std::enable_shared_from_this<session>
{
public:
    session(tcp::socket socket, boost::asio::io_context &io):
        socket_(std::move(socket)),
        socket_server(io),
        buffer_(2048),
        buffer_2(2048),
        resolver_(io)
    {}
      void start()
    {
        first_greetings();
    }

private:
    tcp::socket socket_;
    tcp::socket socket_server;
    std::vector<unsigned char>buffer_;
    std::vector<unsigned char>buffer_2;
    std::string hostname;
    std::string port;
    tcp::resolver resolver_;
    std::map<std::string,std::string>logpas{{"admin","password"}};
    void first_greetings()
     {
         auto self(shared_from_this());
         std::cerr<<"first_greetings\n";
         boost::asio::async_read(socket_, boost::asio::buffer(buffer_2, 2),
                                 [this, self](boost::system::error_code ec, std::size_t)
         {
             if(!ec)
             {
                 if (buffer_2[0] != 0x05)
                 {
                     return;
                 }
                 uint amount_of_number_auth = buffer_2[1];
                 first_greetings_2(amount_of_number_auth);

             }
         });
     }
     void first_greetings_2(uint numb){
         auto self(shared_from_this());
         std::cerr<<"first_greetings_2\n";
         boost::asio::async_read(socket_, boost::asio::buffer(buffer_2, numb),
                                 [this, self, numb](boost::system::error_code ec, std::size_t)
         {
            if(!ec)
            {
                buffer_[0] = 0x05;
                buffer_[1] = 0xFF;
                for(uint i = 0; i < numb; ++i){
                    if (buffer_2[i] == 0x00)
                    {
                        buffer_[1] = 0x00;
                        break;
                    }
                    else if(buffer_2[i] == 0x02)
                    {
                        buffer_[1] = 0x02;
                        break;
                    }
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
                 if(buffer_[1] == 0x00)
                     read_request();
                 else if(buffer_[1] ==0x02){
                     log_pas();
                 }

             }
         });
     }
     void log_pas(){
         auto self = shared_from_this();
         std::cerr<<"log_pas\n";
         boost::asio::async_read(socket_, boost::asio::buffer(buffer_, 2),
                 [this,self](boost::system::error_code ec, std::size_t)
         {//std::wcerr<<buffer_[1]<<'\n';
             uint amount_in_log = buffer_[1];
             if(!ec)
                 boost::asio::async_read(socket_,boost::asio::buffer(buffer_,amount_in_log+1),
                        [this,self,amount_in_log](boost::system::error_code ec, std::size_t)
                {
                     if(!ec){
                         std::string UNAME(&buffer_[0], &buffer_[amount_in_log]);
                         uint amount_in_pas = buffer_[amount_in_log];
                         boost::asio::async_read(socket_,boost::asio::buffer(buffer_2,amount_in_pas),
                                 [this,self,UNAME,amount_in_pas](boost::system::error_code ec, std::size_t)
                         {
                            if(!ec)
                            {
                                std::string PASSWD(&buffer_2[0], &buffer_2[amount_in_pas]);
                                std::cerr<<(UNAME=="admin")<<' '<<UNAME<<'\n';
                                std::cerr<<(PASSWD=="password")<<' '<<PASSWD<<'\n';
                                std::map<std::string,std::string>::iterator check = logpas.find(UNAME);

                                if((check != logpas.end())&&(check->second==PASSWD))//if he exist in map and his second element==PASSDW
                                {
                                    std::cerr<<"ura\n";
                                    buffer_2[0]=0x01;buffer_2[1]=0x00;
                                    boost::asio::async_write(socket_, boost::asio::buffer(buffer_2, 2),
                                                             [this, self](boost::system::error_code ec, std::size_t len)
                                    {
                                        std::cerr<<len<<'\n';
                                        if(!ec)
                                            read_request();
                                    });
                                }
                            }
                         });
                     }
                });
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
                if(buffer_2[0] != 0x05 || buffer_2[2] != 0x00 || buffer_2[3] == 0x04)//
                {
                    return;
                }
                switch (buffer_2[1]) {
                case 0x01://TCP conection
                    if(buffer_2[3]==0x01){
                        second_read_request(4);
                    }
                    else if(buffer_2[3]==0x04){
                        second_read_request(16);
                    }
                    else if(buffer_2[3]==0x03){
                        boost::asio::async_read(socket_, boost::asio::buffer(buffer_, 1),
                                                [this,self](boost::system::error_code ec, std::size_t)
                        {
                            if(!ec){
                            second_read_request(buffer_[0]);}
                        });
                    }
                    break;
                case 0x02://binding
                    break;
                case 0x03://udp conection
                    break;
            }
            }
    });
    }
    void second_read_request(size_t counter){
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
                {   hostname = boost::asio::ip::address_v4(boost::endian::endian_load<uint32_t,4,boost::endian::order::big>(&buffer_[0])).to_string();
                    port = std::to_string(boost::endian::endian_load<uint16_t,2,boost::endian::order::big>(&buffer_[counter]));
                    std::cout<<"hostname - "<<hostname<<'\n'<<"port - "<<port<<'\n';
                    boost::asio::ip::tcp::resolver::results_type results;
                    first_connect(results.create(boost::asio::ip::tcp::endpoint(boost::asio::ip::make_address(hostname),std::stoul(port, nullptr, 0)),hostname,port));
                    break;}
                case 0x03://domain name
                {   hostname =  std::string(buffer_.begin(),buffer_.begin()+counter);
                    port = std::to_string(boost::endian::endian_load<uint16_t,2,boost::endian::order::big>(&buffer_[counter]));
                    std::cout<<"hostname - "<<hostname<<'\n'<<"port - "<<port<<'\n';
                    resolve();
                    break;}
                case 0x04://IPv6
                {   std::uint64_t num1;std::uint64_t num2;
                    std::array<unsigned char, 16> bytes;
                    boost::asio::ip::tcp::resolver::results_type results;
                    std::copy(std::begin(buffer_), std::begin(buffer_) + (std::size(buffer_)-2) / 2, (unsigned char*)&num1);
                    std::copy(std::begin(buffer_) + (std::size(buffer_)-2) / 2, std::end(buffer_)-2, (unsigned char*)&num2);
                    boost::endian::big_to_native(num1);
                    boost::endian::big_to_native(num2);
                    std::copy((unsigned char*)&num1, ((unsigned char*)&num1) + 8, bytes.data());
                    std::copy((unsigned char*)&num2, ((unsigned char*)&num2) + 8, bytes.data() + 8);
                    hostname = boost::asio::ip::address_v6(bytes).to_string();
                    port = std::to_string(boost::endian::endian_load<uint16_t,2,boost::endian::order::big>(&buffer_[counter]));
                    first_connect(results.create(boost::asio::ip::tcp::endpoint(boost::asio::ip::make_address(hostname),std::stoul(port,nullptr,0)),hostname,port));
                    break;}
                default://error on addres
                    break;
                }
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
                    first_connect(result);
                }
            });
    }
    void first_connect(tcp::resolver::results_type results){
        auto self(shared_from_this());
        std::cerr<<"first_connect\n";
        boost::asio::async_connect(socket_server, results,
                                   [this, self](const boost::system::error_code& ec, const tcp::endpoint&)
        {
            switch (ec.value()) {
                case boost::system::errc::success:
                    buffer_[1] = 0x00;
                    break;
                case boost::system::errc::network_down:
                    buffer_[1] = 0x03;
                    break;
                case boost::system::errc::host_unreachable:
                    buffer_[1] = 0x04;
                    break;
                case boost::system::errc::connection_aborted:
                    buffer_[1] = 0x05;
                    break;
                case boost::system::errc::timed_out:
                    buffer_[1] = 0x06;
                    break;
                case boost::system::errc::address_not_available:
                    buffer_[1] = 0x08;
                    break;
                default:
                    buffer_[1] = 0x01;
                    break;
            }
            second_response();

        });
    }
    void second_response(){
        auto self(shared_from_this());
        std::cerr<<"second_response\n";
        buffer_[0] = 0x05; buffer_[2] = 0x00; buffer_[3] = 0x01;
        u_char net_port = socket_server.remote_endpoint().port();
        boost::endian::endian_store<uint32_t, 4, boost::endian::order::little>((u_char*)socket_server.remote_endpoint().address().to_v4().to_string().c_str(),buffer_[4]);
        boost::endian::endian_store<uint16_t, 2, boost::endian::order::little>(&net_port, buffer_[8]);
        boost::asio::async_write(socket_, boost::asio::buffer(buffer_, 10),
                                 [this,self](const boost::system::error_code& ec, size_t)
        {
            if (!ec)
            {
                if (buffer_[1] == 0x00)
                {
                    read_from_client();
                    read_from_server();
                }
            }
        }
        );
    };
    void read_from_client(){
        auto self(shared_from_this());
        socket_.async_read_some(boost::asio::buffer(buffer_),[this,self](const boost::system::error_code& ec, size_t length){
            if(!ec || length != 0)
            {
                write_to_server(length);
            }
            else if (ec != boost::asio::error::eof)
            {
                socket_.close(); socket_server.close();
            }
            else
            {
                socket_server.shutdown(boost::asio::ip::tcp::socket::shutdown_receive);
            }

            std::cerr<<ec.message()<<"\n"<<"in thread "<<std::this_thread::get_id()<<'\n';
        });
    }
    void read_from_server(){
        auto self(shared_from_this());
        socket_server.async_read_some(boost::asio::buffer(buffer_2),[this,self](const boost::system::error_code& ec, size_t length){
            if(!ec || length != 0)
            {
                write_to_client(length);
            }
            else if (ec != boost::asio::error::eof)
            {
                socket_.close(); socket_server.close();
            }
            else
            {
                socket_.shutdown(boost::asio::ip::tcp::socket::shutdown_receive);
            }

        });
    }
    void write_to_server(size_t len){
        auto self(shared_from_this());
        boost::asio::async_write(socket_server, boost::asio::buffer(buffer_, len),[this,self](const boost::system::error_code& ec, size_t)
        {
            if(!ec){
                read_from_client();
            }
        });
}
    void write_to_client(size_t len){
        auto self(shared_from_this());
        boost::asio::async_write(socket_, boost::asio::buffer(buffer_2, len),[this,self](const boost::system::error_code& ec, size_t)
        {
            if(!ec){
                read_from_server();
            }
        });
    }
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
