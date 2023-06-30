#ifndef ENCRYPTEDCOM
#define ENCRYPTEDCOM
#include <stdint.h>
#include <TransistorNoiseSource.h>
#include <string.h>
#include <Vector.h>
#include <bitset>
#include "comIO.hpp"
#include "keys.hpp"
#include "connection.hpp"
#include "encryptor.hpp"
#include "HugeUInt.hpp"

namespace	communication
{
  constexpr auto MaxUintSizeComIO = 256;
  constexpr auto HeaderSizeInByte = 16;
  constexpr auto LineSizeInUint = 4;
  constexpr auto OneMsgSize = 256;
  constexpr auto DataHeaderDescritption = "Data";
  constexpr auto DataHeaderDescritptionSize = 4;
  constexpr auto HeaderSequenceSizeInUint = 3;
  class EncryptedCom
  {
  protected:
    void inputsReceive(uint* data);
    void outputsSend(uint* data);
    wifi::connection _connection;
    encrypt::Keys _keys;
    encrypt::Encryptor _encryptor;
    Vector<digitalInput*> _digitalsInputs;
    Vector<analogInput*> _analogInputs;
    Vector<digitalOutput*> _digitalsOutputs;
    Vector<analogOutput*> _analogOutputs;
    utils::Huge3Uint _serverSequence, _arduinoSequence;
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
    bool controlHeader(uint8_t * header);
    int makeHeader(uint8_t * data);
    int receive(uint8_t * data, int dataSize);
    int receive(String &message, int maxLen = 255);
  };

}
#endif