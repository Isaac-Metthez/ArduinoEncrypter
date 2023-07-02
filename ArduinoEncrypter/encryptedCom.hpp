#ifndef ENCRYPTEDCOM
#define ENCRYPTEDCOM
#include "com.hpp"
#include "encryptor.hpp"
#include "keys.hpp"

namespace	communication
{
  class EncryptedCom : public Com 
  {
  protected:
    encrypt::Keys _keys;
    encrypt::Encryptor _encryptor;
  public:
    void setup() override;
    void send(const uint8_t * data, const int dataSize) override;
    void send(const String &message) override;
    int receive(uint8_t * data, int dataSize) override;
    int receive(String &message, int maxLen = 255) override;
  };
}
#endif