#ifndef CONNECTION
#define CONNECTION
#include <SPI.h>
#include <WiFiNINA.h>
#include <utility/ProgMemUtil.h>
#include "wifi_identifier.hpp"

namespace	wifi
{
  constexpr char *IP = "192.168.127.121";
  constexpr uint16_t Port = 20582;
  class connection
  {
  protected:
    WiFiClient _client;
  public:
    void connect();

    WiFiClient &getClient() {return _client;};
  };
}
#endif