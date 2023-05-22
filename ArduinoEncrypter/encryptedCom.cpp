#include "encryptedCom.hpp"

namespace	communication
{
  EncryptedCom::EncryptedCom(): _encryptor(_keys)
  {}

  void EncryptedCom::setup()
  {
    _connection.connect();

    _keys.init(TransistorNoiseSource(A1));

    _keys.agree(_connection.getClient());
  }
  
  void EncryptedCom::loop()
  {
  }
}