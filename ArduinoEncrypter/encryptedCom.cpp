#include "encryptedCom.hpp"

namespace	communication
{
  EncryptedCom::EncryptedCom()
  {}

  void EncryptedCom::setup()
  {
    _connection.connect();

    _keys.init(TransistorNoiseSource(A1));

    _keys.agree(_connection.getClient());

    _encryptor.setKey(_keys.getSharedKey());
  }
  
  void EncryptedCom::loop()
  {
  }
  
  void EncryptedCom::send()
  {
    uint8_t output[64];
    int size = _encryptor.encrypt("encryted works ?", output);
    _connection.getClient().write(output,size);
    size = _encryptor.encrypt("encryted works, I hope so but it's never sure... I try with many", output);
    _connection.getClient().write(output,size);
    size = _encryptor.encrypt("block", output);
    _connection.getClient().write(output,size);
  }
  
  void EncryptedCom::receive()
  {
  }
}