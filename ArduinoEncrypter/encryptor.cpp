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
  
  int Encryptor::encrypt(const String &message , uint8_t *buffer)
  {
    uint8_t *output = buffer;
    const int *plaintext =  (const int*)message.c_str();
    int blockToEncrypt[BlockSize/sizeof(int)];
    int blocksQuantity = message.length()/BlockSize;
    int notToStuff = message.length()%BlockSize;
    int result = 0;
    for (int j = 0; j <= blocksQuantity ; j++)
    {
      if (j == blocksQuantity )
      {
        if (notToStuff == 0)
        {
          continue;   
        }
        for (uint8_t i = 0; i < BlockSize ; i++)
        {
          uint8_t toXOR =  i < notToStuff ? ((uint8_t*)plaintext)[i] : 0;
          ((uint8_t*)blockToEncrypt)[i] = toXOR ^ ((uint8_t*)lastCipherTextEncrypt)[i];
        }
      }
      else
      {     
        for (uint8_t i = 0; i < BlockSize/sizeof(int) ; i++)
        {
          blockToEncrypt[i] = plaintext[i] ^ lastCipherTextEncrypt[i];
        }
      }
      _aes.encryptBlock(output,(uint8_t*)blockToEncrypt);
      memcpy(lastCipherTextEncrypt,output, BlockSize);
      output += BlockSize;
      plaintext += BlockSize/sizeof(int);
      result += BlockSize;
    }
    return result;
  }
  
  int Encryptor::decrypt(uint8_t *buffer,const String &message , int size)
  {
    return 0;
    // _aes.decryptBlock();
  }
}