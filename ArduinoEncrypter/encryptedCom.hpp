#ifndef ENCRYPTEDCOM
#define ENCRYPTEDCOM
#include <stdint.h>
#include <TransistorNoiseSource.h>
#include <string.h>
#include <Vector.h>
#include "comIO.hpp"
#include "keys.hpp"
#include "connection.hpp"
#include "encryptor.hpp"

namespace	communication
{
  class EncryptedCom
  {
  protected:
    wifi::connection _connection;
    encrypt::Keys _keys;
    encrypt::Encryptor _encryptor;
    Vector<digitalInput*> _digitalsInputs;
    Vector<analogInput*> _analogInputs;
    Vector<digitalOutput*> _digitalsOutputs;
    Vector<analogOutput*> _analogOutputs;
  public:
  EncryptedCom();
  void setup();
  void loop();
  void AddInput(digitalInput* Input){_digitalsInputs.push_back( Input);}
  void AddInput(analogInput* Input){_analogInputs.push_back( Input);}
  void AddOutput(digitalOutput* Output){_digitalsOutputs.push_back( Output);}
  void AddOutput(analogOutput* Output){_analogOutputs.push_back( Output);}
  void send(const uint8_t * data, const int dataSize);
  void send(const String &message);
  int receive(uint8_t * data, int dataSize);
  int receive(String &message, int maxLen = 255);
  };

}
#endif