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
  bool test;
}

void setup()
{
  Serial.begin(9600);
  pinMode(constants::ledPin, OUTPUT);


  globals::com.AddInput(new communication::digitalInput([](bool value) -> void { digitalWrite(constants::ledPin, value);}));
  // globals::com.AddOutput(new communication::digitalOutput([]() -> bool { return  globals::test;}));
  // globals::com.AddOutput(new communication::digitalOutput([]() -> bool { return  globals::test;}));
  // globals::com.AddOutput(new communication::digitalOutput([]() -> bool { return  globals::test;}));
  // globals::com.AddOutput(new communication::digitalOutput([]() -> bool { return  globals::test;}));
  // globals::com.AddOutput(new communication::digitalOutput([]() -> bool { return  !globals::test;}));
  // globals::com.AddOutput(new communication::digitalOutput([]() -> bool { return  !globals::test;}));
  // globals::com.AddOutput(new communication::digitalOutput([]() -> bool { return  !globals::test;}));
  // globals::com.AddOutput(new communication::digitalOutput([]() -> bool { return  !globals::test;}));
  // globals::com.AddOutput(new communication::digitalOutput([]() -> bool { return  globals::test;}));
  // globals::com.AddOutput(new communication::digitalOutput([]() -> bool { return  globals::test;}));
  // globals::com.AddOutput(new communication::digitalOutput([]() -> bool { return  globals::test;}));
  // globals::com.AddOutput(new communication::digitalOutput([]() -> bool { return  globals::test;}));
  // globals::com.AddOutput(new communication::digitalOutput([]() -> bool { return  !globals::test;}));
  // globals::com.AddOutput(new communication::digitalOutput([]() -> bool { return  !globals::test;}));
  // globals::com.AddOutput(new communication::digitalOutput([]() -> bool { return  !globals::test;}));
  // globals::com.AddOutput(new communication::digitalOutput([]() -> bool { return  !globals::test;}));
  globals::com.AddOutput(new communication::analogOutput([]() -> int { return  255;}));

  globals::com.setConditionSendOutput([]() -> bool { return  true;});
  globals::com.setup();

}
void loop()
{
  delay(1000);
  globals::com.loop();
  globals::test = !globals::test;



}
