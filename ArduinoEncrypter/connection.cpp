#include "connection.hpp"

namespace	wifi
{
  void connection::connect()
  {
    int status = WL_IDLE_STATUS;
    while (true)
    {
      // Connect to network:
      status = WiFi.begin(wifi::identifier::SSID, wifi::identifier::PassWord);

      // wait 2 seconds for connection:
      if (status != WL_CONNECTED)
        break;
      else
        delay(2000);
    }
    _client.connect(IP,Port);
  }
}