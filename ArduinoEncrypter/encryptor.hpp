#ifndef CIPHER
#define CIPHER
#include <Crypto.h>
#include <AES.h>
#include <string.h>
#include "keys.hpp"

namespace	encrypt
{
  class Encryptor
  {
  protected:
    AES256 _aes;
    Keys &_keys;
  public:
    Encryptor(Keys &keys);
    void encypt( String &message , uint8_t *buffer);
    void decrypt(uint8_t *buffer, String &message);
  };
}
#endif