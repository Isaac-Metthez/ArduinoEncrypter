#ifndef CIPHER
#define CIPHER
#include <Crypto.h>
#include <AES.h>
#include <string.h>
#include "keys.hpp"

namespace	encrypt
{
  constexpr auto BlockSize = 16;
  class Encryptor
  {
  protected:
    AES256 _aes;
    int lastCipherTextEncrypt[BlockSize/sizeof(int)] ;
    int lastCipherTextDecrypt[BlockSize/sizeof(int)]  ;
  public:
    Encryptor(uint8_t fillInitialVector = 0);
    Encryptor(const uint8_t * initialVector);

    void setKey(const uint8_t *sharedKey);
    int encrypt(const String &message , uint8_t *buffer);
    int decrypt(uint8_t *buffer,const String &message, int size);
  };
}
#endif