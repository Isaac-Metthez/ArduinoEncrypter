#ifndef ENCRYPTEDCOM
#define ENCRYPTEDCOM
#include <stdint.h>
#include <TransistorNoiseSource.h>
#include <string.h>
#include "keys.hpp"
#include "connection.hpp"
#include "encryptor.hpp"

namespace	communication
{
  class EncryptedCom
  {
  protected:
    wifi::connection _connection;
    encrypt::Keys _keys;
    encrypt::Encryptor _encryptor;
  public:
  EncryptedCom();
  void setup();
  void loop();
  void send(const uint8_t * data, const int dataSize);
  void send(const String &message);
  int receive(uint8_t * data, int dataSize);
  int receive(String &message, int maxLen = 255);
  };
}
#endif