#include "server.cpp"

int main(int argc, char* argv[])
{
  try
  {
    if (argc != 2)
    {
      std::cerr << "error\n";
      //return 1;
    }
    argv[1]="1234";
    std::size_t max_threads = 2;
    std::vector<std::thread>thread_pool;
    boost::asio::io_context io_context(max_threads);
    server server_(io_context, std::atoi(argv[1]));
    for(size_t i = 0; i<max_threads;i++){thread_pool.emplace_back(std::thread([&](){io_context.run();}));};
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
