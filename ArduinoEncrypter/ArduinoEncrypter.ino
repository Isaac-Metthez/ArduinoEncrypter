#include <SPI.h>
#include <WiFiNINA.h>
#include <Crypto.h>
#include <SHA256.h>
#include <AES.h>
#include <RNG.h>
#include <Curve25519.h>
#include <Ed25519.h>
#include <utility/ProgMemUtil.h>
#include <TransistorNoiseSource.h>
#include <string.h>
#include "wifi_identifier.hpp"
#include "keys.hpp"
#include "connection.hpp"

namespace	globals
{    
  wifi::connection connection;
  encrypt::keys keys; // pin A1 is reserved for keygen
}

void setup()
{

  globals::connection.connect();

  globals::keys.init(TransistorNoiseSource(A1));

  globals::keys.askServerPub(globals::connection.getClient());
  globals::keys.agree(globals::connection.getClient());
}
void loop()
{
}
