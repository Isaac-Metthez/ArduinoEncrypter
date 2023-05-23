#include "encryptor.hpp"
namespace	encrypt
{
  Encryptor::Encryptor(uint8_t fillInitialVector)
  {
      memset(lastCipherTextEncrypt,fillInitialVector, BlockSize);
      memset(lastCipherTextDecrypt,fillInitialVector, BlockSize);
  }
  Encryptor::Encryptor(const uint8_t *initialVector)
  {
      memcpy(lastCipherTextEncrypt,initialVector, BlockSize);
      memcpy(lastCipherTextDecrypt,initialVector, BlockSize);
  }

  void Encryptor::setKey(const uint8_t *sharedKey)
  {
    _aes.setKey(sharedKey , KeySize);
  }

  int Encryptor::encrypt(const uint8_t *plaintext, uint8_t *ciphertext, int size)
  {
    int blockToEncrypt[BlockSize/sizeof(int)];
    int blocksQuantity = size/BlockSize;
    int notToStuff = size%BlockSize;
    int result = 0;
    for (int j = 0; j <= blocksQuantity; j++)
    {
      if (j == blocksQuantity)
      {
        if (notToStuff == 0)
        {
          continue;   
        }
        for (uint8_t i = 0; i < BlockSize ; i++)
        {
          uint8_t toXOR =  i < notToStuff ? plaintext[i] : 0;
          ((uint8_t*)blockToEncrypt)[i] = toXOR ^ ((uint8_t*)lastCipherTextEncrypt)[i];
        }
      }
      else
      {     
        for (uint8_t i = 0; i < BlockSize/sizeof(int) ; i++)
        {
          blockToEncrypt[i] =((const int*)plaintext)[i] ^ lastCipherTextEncrypt[i];
        }
      }
      _aes.encryptBlock(ciphertext,(uint8_t*)blockToEncrypt);
      memcpy(lastCipherTextEncrypt,ciphertext, BlockSize);
      ciphertext += BlockSize;
      plaintext += BlockSize;
      result += BlockSize;
    }
    return result;
  }
  
  void Encryptor::decrypt(const uint8_t *ciphertext, uint8_t *plaintext, int size)
  {
    int blockDecrypted[BlockSize/sizeof(int)];
    for (int j = 0; j < size/BlockSize; j++)
    {     
      _aes.decryptBlock((uint8_t*)blockDecrypted,ciphertext);
      for (uint8_t i = 0; i < BlockSize/sizeof(int) ; i++)
      {
        ((int*)plaintext)[i] = blockDecrypted[i] ^ lastCipherTextDecrypt[i];
      }
      memcpy(lastCipherTextDecrypt,ciphertext, BlockSize);
      ciphertext += BlockSize;
      plaintext += BlockSize;
    }
  }
}