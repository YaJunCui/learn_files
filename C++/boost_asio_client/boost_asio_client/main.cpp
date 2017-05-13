#define _WIN32_WINDOWS 0x0501
#define _CRT_SECURE_NO_WARNINGS
#define _WINSOCK_DEPRECATED_NO_WARNINGS

#include "Connector.hpp"

#include <iostream>
#include <string>

using namespace std;

#define HEAD_LEN  4

int main()
{
  io_service ios;
  boost::asio::io_service::work work(ios);
  boost::thread thd([&ios] {ios.run(); });

  Connector conn(ios, "127.0.0.1", 9900);
  conn.Start();

  string str;
  if (!conn.IsConnected())
  {
    cin >> str;
    return -1;
  }

  const int len = 512;
  char line[len] = "";

  while (cin >> str)
  {
    char header[HEAD_LEN] = {};
    int totalLen = str.length() + 1 + HEAD_LEN;
    std::sprintf(header, "%d", totalLen);

    memcpy(line, header, HEAD_LEN);
    memcpy(line + HEAD_LEN, str.c_str(), str.length() + 1);

    conn.Send(line, totalLen);
  }

  return 0;
}