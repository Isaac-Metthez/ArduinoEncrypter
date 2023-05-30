#include <SPI.h>
#include <string.h>
#include "encryptedCom.hpp"
namespace	constants
{  
  constexpr auto ledPin = 13;
}
namespace	globals
{  
  communication::EncryptedCom com;
  bool test;
}

void setup()
{
  pinMode(constants::ledPin, OUTPUT);

  // globals::com.setup();

  globals::com.AddInput(new communication::digitalInput([](bool value) -> void { digitalWrite(constants::ledPin, value);}));
  globals::com.AddOutput(new communication::digitalOutput([]() -> bool { return  globals::test;}));


}
void loop()
{
  globals::com.loop();



}
