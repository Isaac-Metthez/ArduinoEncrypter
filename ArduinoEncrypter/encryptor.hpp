#ifndef CIPHER
#define CIPHER
#include <Crypto.h>
#include <AES.h>
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
    int encrypt(const uint8_t *plaintext, uint8_t *ciphertext, int size);
    void decrypt(const uint8_t *ciphertext, uint8_t *plaintext, int size);
  };
}
#endif