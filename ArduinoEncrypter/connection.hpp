#ifndef CONNECTION_hpp
#define CONNECTION_hpp
#include <SPI.h>
#include <WiFiNINA.h>
#include <utility/ProgMemUtil.h>
#include "wifi_identifier.hpp"

namespace	wifi
{
  constexpr char *IP = "192.168.77.121";
  constexpr uint16_t Port = 20582;
  class connection
  {
  protected:
    WiFiClient _client;
  public:
    connection() = default;
    ~connection() = default;

    void connect();

    WiFiClient &getClient() {return _client;};
  };
}
#endif