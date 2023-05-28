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
    delay(500);
    for (digitalInput* input : _digitalsInputs)
    {
        input->setValue(true);
    }
    delay(1000);
    for (digitalInput* input : _digitalsInputs)
    {
        input->setValue(false);
    }

  }

  void EncryptedCom::send(const uint8_t * data, const int dataSize)
  {
    constexpr auto OneMsgSize = 256;
    uint8_t send[OneMsgSize];
    int numberMessage = dataSize/OneMsgSize;

    for (int i = 0 ; i <= numberMessage; i++)
    {
      int messageLength = OneMsgSize;
      if (i == numberMessage)
        messageLength = dataSize%OneMsgSize == 0 ? OneMsgSize : dataSize%OneMsgSize;
      int size = _encryptor.encrypt(data, send, messageLength);
      _connection.getClient().write(send, size);
      data += OneMsgSize;
    }
  }

  void EncryptedCom::send(const String &message)
  {
    constexpr auto OneMsgSize = 256;
    uint8_t send[OneMsgSize];
    int numberMessage = message.length()/OneMsgSize;
    const uint8_t* strMessage = (uint8_t*)message.c_str();

    for (int i = 0 ; i <= numberMessage; i++)
    {
      int messageLength = OneMsgSize;
      if (i == numberMessage)
        messageLength = message.length()%OneMsgSize == 0 ? OneMsgSize : message.length()%OneMsgSize;
      int size = _encryptor.encrypt(strMessage, send, messageLength);
      _connection.getClient().write(send, size);
      strMessage += OneMsgSize;
    }
  }

  int EncryptedCom::receive(uint8_t * data, int dataSize)
  {
    uint8_t received[encrypt::BlockSize];
    int i = 0;
    while (_connection.getClient().available() >= encrypt::BlockSize 
          && i + encrypt::BlockSize <= dataSize)
    {
      _connection.getClient().read(received,encrypt::BlockSize);
      _encryptor.decrypt(received, data, encrypt::BlockSize);
      data += encrypt::BlockSize;
      i += encrypt::BlockSize;
    }
    return i;
  }

  int EncryptedCom::receive(String &message, int maxLen)
  {
    int i = 0;
    uint8_t received[encrypt::BlockSize];
    uint8_t decrypted[encrypt::BlockSize + 1];
    decrypted[encrypt::BlockSize] = 0;
    message = "";
    
    while (_connection.getClient().available() >= encrypt::BlockSize 
          && i + encrypt::BlockSize <= maxLen
          && message.length() == i)
    {
      _connection.getClient().read(received,encrypt::BlockSize);
      _encryptor.decrypt(received, decrypted, encrypt::BlockSize);
      message += (char*)decrypted;
      i += encrypt::BlockSize;
    }
    return message.length();
  }
}