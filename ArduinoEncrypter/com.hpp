#ifndef COM
#define COM
#include <stdint.h>
#include <TransistorNoiseSource.h>
#include <string.h>
#include <bitset>
#include <Vector.h>
#include "api/Common.h"
#include "comIO.hpp"
#include "connection.hpp"
#include "hugeUInt.hpp"

namespace	communication
{
  constexpr auto MaxIO = 128;
  constexpr auto MaxUintSizeComIO = 256;
  constexpr auto LineSizeInUint = 4;
  constexpr auto BitPerLine = 128;
  constexpr auto BytePerLine = 16;
  constexpr auto BitPerByte = 8;
  constexpr auto HeaderSizeInByte = BytePerLine;
  constexpr auto BitInUint = 32;
  constexpr auto OneMsgSize = 256;
  constexpr auto DataHeaderDescritption = "Data";
  constexpr auto DataHeaderDescritptionSize = 4;
  constexpr auto HeaderSequenceSizeInUint = 3;

  class Com
  {
  protected:
    virtual unsigned int inputsReceive(unsigned int* data);
    virtual unsigned int outputsSend(unsigned int* data);
    wifi::connection _connection;

    // mem for static memory vector
    digitalInput* arrayDI[MaxIO];
    analogInput* arrayAI[MaxIO];
    digitalOutput* arrayDO[MaxIO];
    analogOutput* arrayAO[MaxIO];
    Vector<digitalInput*> _digitalsInputs;
    Vector<analogInput*> _analogInputs;
    Vector<digitalOutput*> _digitalsOutputs;
    Vector<analogOutput*> _analogOutputs;
    utils::Huge3Uint _serverSequence, _arduinoSequence;
    unsigned int _inputsMsgSize = 0, _outputsMsgSize = 0;
    bool (*_sendCondition)();
    
  public:
    Com();
    virtual ~Com();
    virtual void setup();
    virtual void loop();

    void setConditionSendOutput(bool (*sendCondition)()){_sendCondition = sendCondition;};
    void AddInput(digitalInput* Input);
    void AddInput(analogInput* Input);
    void AddOutput(digitalOutput* Output);
    void AddOutput(analogOutput* Output);
    
    virtual void send(const uint8_t * data, const int dataSize);
    virtual void send(const String &message);
    virtual bool controlHeader(uint8_t * header);
    virtual int makeHeader(uint8_t * data);
    virtual int receive(uint8_t * data, int dataSize);
    virtual int receive(String &message, int maxLen = 255);
  };
}
#endif
