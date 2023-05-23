#include <stdint.h>
#ifndef ENCRYPTEDCOM
#define ENCRYPTEDCOM
#include <TransistorNoiseSource.h>
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
  void send();
  void receive();
  };
}
#endif