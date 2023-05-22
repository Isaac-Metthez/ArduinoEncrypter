#include "encryptor.hpp"
namespace	encrypt
{
  Encryptor::Encryptor(Keys &keys): _keys{keys}
  {}

  void Encryptor::encypt(String &message , uint8_t *buffer)
  {
    _aes.setKey(_keys.getSharedKey() , KeySize);
    // _aes.encryptBlock
  }
  
  void Encryptor::decrypt(uint8_t *buffer, String &message )
  {
    _aes.setKey(_keys.getSharedKey() , KeySize);
    
  }
}