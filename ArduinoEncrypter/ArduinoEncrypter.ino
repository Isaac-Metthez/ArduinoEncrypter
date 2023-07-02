#include <SPI.h>
#include <string.h>
#include "com.hpp"
#include "encryptedCom.hpp"
#include "secureCom.hpp"

namespace	constants
{  
  constexpr auto ledPin = 13;
}
namespace	globals
{  
  communication::Com com;
  bool test = false;
  bool send = false;
  int counter = 0;
}

void setup()
{
  Serial.begin(9600);
  pinMode(constants::ledPin, OUTPUT);
  globals::com.setConditionSendOutput([]() -> bool { if (globals::send) {globals::send = false; return true;} return false;});


  globals::com.AddInput(new communication::digitalInput([](bool value) -> void { globals::send = value;}));
  globals::com.AddInput(new communication::digitalInput([](bool value) -> void { digitalWrite(constants::ledPin, value);}));

  globals::com.AddOutput(new communication::digitalOutput([]() -> bool { globals::test = !globals::test; return  globals::test;}));

  globals::com.AddOutput(new communication::analogOutput([]() -> int { return  ++globals::counter;}));

  globals::com.setup();
}
void loop()
{
  globals::com.loop();
  globals::test = !globals::test;



}
