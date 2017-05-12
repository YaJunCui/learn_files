#define _WIN32_WINDOWS 0x0501
#define _CRT_SECURE_NO_WARNINGS
#define _WINSOCK_DEPRECATED_NO_WARNINGS

#include "Server.hpp"

void TestServer()
{
  io_service ios;
  //boost::asio::io_service::work work(ios);
  //std::thread thd([&ios]{ios.run(); }); 

  Server server(ios, 9900);
  server.Accept();
  ios.run();

  //thd.join();
}

int main()
{
  TestServer();

  return 0;
}