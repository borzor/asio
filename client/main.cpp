#include "master.hpp"

int main()//int argc, char* argv[]
{
    try
    {
       /* if (argc != 7   )
        {
          throw std::runtime_error("Correct format: [listen-port] [target-port] [number_of_threads] [buffer_size] [number_of_clients] [time_on_test]");
        }

        ushort listen_port=static_cast<unsigned short>(std::strtoul(argv[1],nullptr,0));
        ushort target_port=static_cast<unsigned short>(std::strtoul(argv[2],nullptr,0));
        double number_of_threads=std::strtod(argv[3],nullptr);
        std::size_t buffer_size=std::strtoul(argv[4],nullptr,0);
        double number_of_clients=std::strtod(argv[5],nullptr);
        std::size_t time=std::strtoul(argv[6],nullptr,0);*/
        ushort listen_port = 1234;
        ushort target_port = 1235;
        double number_of_threads=2;
        size_t buffer_size=8;
        double number_of_clients=8;
        size_t time=2000000;



        std::vector<std::thread> thread_pool;
        double counter=number_of_clients/number_of_threads;
        //uint number_of_connections = 0;
        std::vector<std::vector<client>> client_(static_cast<std::size_t>(number_of_threads));
        if(listen_port <= 0 || listen_port > 65535){
            throw std::runtime_error("Listen port should be more then 1 and less then 65535");}
        if(target_port <= 0 || target_port > 65535){
            throw std::runtime_error("Port should be more then 1 and less then 65535");}
        if(number_of_threads<1 || number_of_threads>std::thread::hardware_concurrency()){
            std::cerr<<"Setting amount of threads to recommended\n";
            number_of_threads = std::thread::hardware_concurrency();}
        if(counter!=std::ceil(counter)){
            std::cerr<<"[number_of_threads] should be divider for [number_of_clients]\n";
            return 0;
        }

        for(int i=0;i<number_of_threads;i++){
            for(std::size_t j=0;j<counter;j++){
                client_[static_cast<std::size_t>(i)].emplace_back(client(listen_port, target_port, 0x00, "127.0.0.1", buffer_size));
            }
        }
        //test_(client_[0], time);
        for(std::size_t i=0;i<number_of_threads;i++)
        {
            thread_pool.emplace_back(std::thread(test_,std::ref(client_[i]),time));
        }
        for(auto &thread:thread_pool)
        {
            thread.join();
        }
    }
    catch (std::exception& e)
    {
      std::cerr << "Exception: " << e.what() << "\n";
    }

    return 0;
}
