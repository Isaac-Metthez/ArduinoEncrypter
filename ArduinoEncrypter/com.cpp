#include "api/Common.h"
#include "com.hpp"

namespace	communication
{
  Com::Com()
  {
    _digitalsInputs.setStorage(arrayDI);
    _analogInputs.setStorage(arrayAI);
    _digitalsOutputs.setStorage(arrayDO);
    _analogOutputs.setStorage(arrayAO);
  }
  Com::~Com()
  {
  }
  void Com::AddInput(digitalInput* Input)
  {
    _digitalsInputs.push_back( Input);
    _inputsMsgSize = BytePerLine * ( 1 
      + _digitalsInputs.size()%BytePerLine == 0 ? _digitalsInputs.size()/BytePerLine :  1 + _digitalsInputs.size()/BytePerLine
      + _analogInputs.size()%BytePerLine == 0 ? _analogInputs.size()/BytePerLine :  1 + _analogInputs.size()/BytePerLine);
  }
  void Com::AddInput(analogInput* Input)
  {
    _analogInputs.push_back( Input);
    _inputsMsgSize = BytePerLine * ( 1 
      + _digitalsInputs.size()%BytePerLine == 0 ? _digitalsInputs.size()/BytePerLine :  1 + _digitalsInputs.size()/BytePerLine
      + _analogInputs.size()%BytePerLine == 0 ? _analogInputs.size()/BytePerLine :  1 + _analogInputs.size()/BytePerLine);
  }
  void Com::AddOutput(digitalOutput* Output)
  {
    _digitalsOutputs.push_back(Output);
    _outputsMsgSize = BytePerLine * (1 
      + _digitalsOutputs.size()%BytePerLine == 0 ? _digitalsOutputs.size()/BytePerLine :  1 + _digitalsOutputs.size()/BytePerLine
      + _analogOutputs.size()%BytePerLine == 0 ? _analogOutputs.size()/BytePerLine :  1 + _analogOutputs.size()/BytePerLine);
    }
  void Com::AddOutput(analogOutput* Output)
  {
    _analogOutputs.push_back(Output);
    _outputsMsgSize = BytePerLine * ( 1 
      + _digitalsOutputs.size()%BytePerLine == 0 ? _digitalsOutputs.size()/BytePerLine :  1 + _digitalsOutputs.size()/BytePerLine
      + _analogOutputs.size()%BytePerLine == 0 ? _analogOutputs.size()/BytePerLine :  1 + _analogOutputs.size()/BytePerLine);
  }

  void Com::setup()
  {
    _connection.connect();
  }

  void Com::loop()
  {  
    unsigned int dataBuffer[MaxUintSizeComIO];
    if ((!_sendCondition || _sendCondition())
      && !(_digitalsOutputs.empty() &&  _analogOutputs.empty()))
      outputsSend(dataBuffer);

    int received = receive((uint8_t*)dataBuffer, HeaderSizeInByte);
    if (received)
      if (controlHeader((uint8_t *)dataBuffer))
        inputsReceive(dataBuffer);
      else
        return ; // to do reset com    
  }

  bool Com::controlHeader(uint8_t * header)
  {
    return (memcmp(header, DataHeaderDescritption, DataHeaderDescritptionSize) == 0
            && ++_serverSequence == (unsigned int*)(header + DataHeaderDescritptionSize));
  }

  int Com::makeHeader(uint8_t * data)
  {
    for (uint8_t i = 0; i < DataHeaderDescritptionSize ; ++i)
      data[i] = DataHeaderDescritption[i];
    ++_arduinoSequence ;
    for (uint8_t i = 0; i < HeaderSequenceSizeInUint ; ++i)
      ((unsigned int*)data)[i+1] = _arduinoSequence[i];
    return LineSizeInUint;
  }

  unsigned int Com::inputsReceive(unsigned int* data)
  {
    Serial.println("inputrec");
    unsigned int blocks = 0;
    unsigned int i = 0;
    for (digitalInput* input : _digitalsInputs)
    { 
      uint8_t bitPos = i++ % BitPerLine;
      if (!bitPos)
      {
        data += LineSizeInUint;
        blocks += BytePerLine;
        while(!receive((uint8_t *)data, BytePerLine));
      }
      input->set(bitRead(((uint8_t*)data)[bitPos/BitPerByte], (BitPerByte - 1) - bitPos%BitPerByte));
    }

    i = 0;
    for (analogInput* input : _analogInputs)
      { 
        uint8_t intPos = i++ % LineSizeInUint;
        if (!intPos)
        {
          data += LineSizeInUint;
          blocks += BytePerLine;
          while(!receive((uint8_t *)data,BytePerLine));
        }
        input->set(data[intPos]);
    }
    return blocks;
  }
  
  unsigned int Com::outputsSend(unsigned int* data)
  {
    unsigned int* startOfData = data;
    makeHeader((uint8_t*)data);

    uint messageLenght = BytePerLine;
    uint i = 0;

    for (digitalOutput* output : _digitalsOutputs)
    { 
      uint8_t bitPos = i++ % BitPerLine;
      if (!bitPos)
      {
        data += LineSizeInUint;
        messageLenght += BytePerLine;
      }
      bitWrite(((uint8_t*)data)[bitPos/BitPerByte], (BitPerByte - 1) - bitPos%BitPerByte, output->get());
    }

    i = 0;
    for (analogOutput* output : _analogOutputs)
    { 
      uint8_t intPos = i++ % LineSizeInUint;
      if (!intPos)
      {
        data += LineSizeInUint;
        messageLenght += BytePerLine;
      }
      data[intPos] = output->get();
    }
    send((uint8_t *)startOfData, messageLenght);
    return messageLenght;
  }

  void Com::send(const uint8_t * data, const int dataSize)
  {
      _connection.getClient().write(data, dataSize);
  }

  void Com::send(const String &message)
  {
      _connection.getClient().write(message.c_str(), message.length());
  }

  int Com::receive(uint8_t * data, int dataSize)
  {
    int i = 0;
    while (_connection.getClient().available()
          && i + BytePerLine <= dataSize)
    {
      _connection.getClient().read(data, BytePerLine);
      data += BytePerLine;
      i += BytePerLine;
    }
    return i;
  }

  int Com::receive(String &message, int maxLen)
  {
    int i = 0;
    uint8_t received[BytePerLine + 1];
    received[BytePerLine] = 0;
    message = "";
    
    while (_connection.getClient().available() >= BytePerLine
          && i + BytePerLine <= maxLen
          && message.length() == i)
    {
      _connection.getClient().read(received, BytePerLine);
      message += (char*)received;
      i += BytePerLine;
    }
    return i;
  }
}