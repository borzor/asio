#include "server.cpp"
#include <charconv>
#include <span>
int main(int argc, char* argv[])
{
  try
  {
    if (argc != 3)
    {
      throw std::runtime_error("Correct format: <listen-port> <number_of_threads>");
    }
    uint16_t port;
    uint number_of_treads;
    std::from_chars<uint16_t>(argv[1],argv[2], port);
    std::from_chars<uint>(argv[2],argv[3],number_of_treads);
    if(!(port>1 && port<65535)){
        throw std::runtime_error("Port should be more then 1 and less then 65535");}
    if(!(number_of_treads>=1 && number_of_treads<=std::thread::hardware_concurrency())){
        std::cerr<<"Setting amount of threads to recommended\n";
        number_of_treads = std::thread::hardware_concurrency();
    }
    std::vector<std::thread>thread_pool;
    boost::asio::io_context io_context(number_of_treads);
    server server_(io_context, port);
    for(size_t i = 0; i<number_of_treads;i++){thread_pool.emplace_back(std::thread([&](){io_context.run();}));};
    for(auto &thread:thread_pool){
        thread.join();
    }
  }
  catch (std::exception& e)
  {
    std::cerr << "Exception: " << e.what() << "\n";
  }

  return 0;
}
