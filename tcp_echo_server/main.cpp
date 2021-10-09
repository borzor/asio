#include "server.cpp"
#include <charconv>

int main(int argc, char **argv)
{
    try
    {
        if (argc != 2)
        {
          throw std::runtime_error("Correct format: <port>");
        }
        uint16_t port;
        std::from_chars<uint16_t>(argv[1],argv[2], port);
        if(port <= 0 || port > 65535){
            throw std::runtime_error("Port should be more then 1 and less then 65535");}
        const int OPEN_MAX = 1024; //sysconf(_SC_OPEN_MAX);   maximum number of opened files
        tcp_server server(port, OPEN_MAX);
        server.start();
    }
    catch (std::exception& e)
    {
      std::cerr << "Exception: " << e.what() << "\n";
    }
    return 0;
}
