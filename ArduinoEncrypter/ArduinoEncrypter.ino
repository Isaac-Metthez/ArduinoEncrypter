#include <SPI.h>
#include "encryptedCom.hpp"

namespace	globals
{    
  communication::EncryptedCom com;
}

void setup()
{
  globals::com.setup();
}
void loop()
{
  globals::com.loop();
}
