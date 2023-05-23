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

  // void EncryptedCom::send(const uint8_t * buffer,const int size)
  // {
  // //   uint8_t output[64];
  // //   const uint8_t pt[] = "encryted works ?";
  // //   const uint8_t pt2[] = "encryted works, I hope so but it's never sure... I try with many blocks";
  // //   int size = _encryptor.encrypt(pt, output, sizeof(pt));
  // //   _connection.getClient().write(output,size);
  // //   size = _encryptor.encrypt(pt2, output, sizeof(pt2));
  // //   _connection.getClient().write(output,size);
  //   // size = _encryptor.encrypt((uint8_t*)"block", output,5);
  //   // _connection.getClient().write(output,size);
  // }

  void EncryptedCom::send(const String &message)
  {
    constexpr auto Size = 256;
    uint8_t output[Size];
    int numberMessage = message.length()/Size;

    for (int i = 0 ; i <= numberMessage; i++)
    {
      int messageLength = Size;
      if (i == numberMessage)
        messageLength = message.length()%Size == 0 ? Size : message.length()%Size;
      int size = _encryptor.encrypt((uint8_t*)message.c_str(), output, messageLength);
      _connection.getClient().write(output,size);
    }
  }

  void EncryptedCom::receive()
  {
  }
}