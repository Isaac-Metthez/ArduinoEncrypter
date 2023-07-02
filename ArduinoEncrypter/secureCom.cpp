#include "secureCom.hpp"

namespace	communication
{

  void SecureCom::loop()
  {   
    unsigned int dataBuffer[MaxUintSizeComIO];

    if ((!_sendCondition || _sendCondition())
      && !(_digitalsOutputs.empty() &&  _analogOutputs.empty()))
      outputsSend(dataBuffer);
    
    int msg = receiveMsg(HeaderSizeInByte + _outputsMsgSize);
    if (msg > 0)
    {
      int received = receive((uint8_t*)dataBuffer, HeaderSizeInByte);
      if (received)
        if (controlHeader((uint8_t*)dataBuffer))
        { 
          inputsReceive(dataBuffer + LineSizeInUint);
        }
        else
          return ; // to do reset com    
    }
    else if (msg < 0)
      return ; // to do reset com
  }
  
  void SecureCom::send(const uint8_t * data, const int dataSize)
  {
    unsigned int send[MaxUintSizeComIO + encrypt::SignSizeInUint];

    int size = _encryptor.encrypt(data, (uint8_t*)send, dataSize);

    Ed25519::sign(
    ((uint8_t*)send) + dataSize,
    _keys.getPrivateKey(), 
    _keys.getPublicKey(), 
    (uint8_t*)send, 
    dataSize);

    _connection.getClient().write((uint8_t*)send, dataSize + encrypt::SignSize);
  }

  void SecureCom::send(const String &message)
  {
    unsigned int send[MaxUintSizeComIO + encrypt::SignSizeInUint];

    int size = _encryptor.encrypt((uint8_t*)message.c_str(), (uint8_t*)send, message.length());

    Ed25519::sign(
    ((uint8_t*)send) + message.length(),
    _keys.getPrivateKey(), 
    _keys.getPublicKey(), 
    (uint8_t*)send, 
    message.length());

    _connection.getClient().write((uint8_t*)send, message.length() + encrypt::SignSize);
  }
  
  int SecureCom::receiveMsg(int dataSize)
  {
    if (_connection.getClient().available() >= dataSize + encrypt::SignSize)
    {
      int size = _connection.getClient().read((uint8_t*)_dataReceived, dataSize + encrypt::SignSize);
      _indexData = 0;
      if (Ed25519::verify(
          ((uint8_t*)_dataReceived) + dataSize, 
          _keys.getServerKey(), 
          (uint8_t*)_dataReceived, 
          dataSize))
          return size;
      else
          return -1;
    }
    return 0;
    
  }
  
  int SecureCom::receive(uint8_t * data, int dataSize)
  {
    if (_indexData + dataSize < _outputsMsgSize)
    {
      _encryptor.decrypt((uint8_t *)_dataReceived, data, encrypt::BlockSize);
      _indexData += dataSize;
      return dataSize;
    }
    else 
      return 0;
  }

  int SecureCom::receive(String &message, int maxLen)
  {
    int i = 0;
    uint8_t decrypted[encrypt::BlockSize + 1];
    decrypted[encrypt::BlockSize] = 0;
    message = "";
    
    while (_indexData + encrypt::BlockSize  < _outputsMsgSize
          && i + encrypt::BlockSize <= maxLen
          && message.length() == i)
    {
      _encryptor.decrypt((uint8_t *)_dataReceived, decrypted, encrypt::BlockSize);
      message += (char*)decrypted;
      i += encrypt::BlockSize;
      _indexData += encrypt::BlockSize;
    }
    return message.length();
  }
}