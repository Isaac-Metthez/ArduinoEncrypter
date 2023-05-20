#include "connection.hpp"

namespace	wifi
{
  void connection::connect()
  {
    int status = WL_IDLE_STATUS;
    while (status != WL_CONNECTED)
    {
      // Connect to network:
      status = WiFi.begin(wifi::identifier::SSID, wifi::identifier::PassWord);

      // wait 2 seconds for connection:
      delay(2000);
    }
    _client.connect(IP,Port);
  }
}