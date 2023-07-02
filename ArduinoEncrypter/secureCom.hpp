#ifndef SECURECOM
#define SECURECOM
#include "encryptedCom.hpp"

namespace	communication
{
  class SecureCom : public EncryptedCom
  {
  protected:  
    unsigned int _dataReceived[MaxUintSizeComIO], _indexData;
  public:
    virtual void loop();
    void send(const uint8_t * data, const int dataSize) override;
    void send(const String &message) override;
    int receive(uint8_t * data, int dataSize) override;
    int receive(String &message, int maxLen = 255) override;
    int receiveMsg(int dataSize);
  };

}
#endif