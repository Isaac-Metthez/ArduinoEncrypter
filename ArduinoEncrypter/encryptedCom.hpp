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
  // void send(uint8_t * buffer,int size);
  void send(const String &message);
  void receive();
  };
}
#endif