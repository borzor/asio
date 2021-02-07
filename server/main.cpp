#include <iostream>
#include <stdio.h>
#include <string.h>
#include <boost/asio.hpp>

int main()
{
    std::vector<char>buffer_2{11, 22, 33, 44, 55};
    std::string str_ = boost::asio::ip::address_v4(ntohl(*(uint32_t*)(&buffer_2[4]))).to_string();
    std::cout<<str_<<'\n';

  return 0;
}
