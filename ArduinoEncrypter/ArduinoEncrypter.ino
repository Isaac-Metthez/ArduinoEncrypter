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

WiFiClient client;

TransistorNoiseSource noise(A1);

void setup()
{
  int status = WL_IDLE_STATUS;
  while (status != WL_CONNECTED)
  {
    // Connect to WPA/WPA2 network:
    status = WiFi.begin(wifi::identifier::SSID, wifi::identifier::PassWord);

    // wait 5 seconds for connection:
    delay(5000);
  }

  client.connect("192.168.77.121",8080);

  client.write("connection estatblished ?");

  
  static uint8_t privateKey[32];
  static uint8_t publicKey[32];
  RNG.begin("TestCurve25519 1.0");
  RNG.addNoiseSource(noise);
  
  client.write("start generating private key");
  Ed25519::generatePrivateKey(privateKey);
  client.write("private key generated, start public key");
  Ed25519::derivePublicKey(publicKey, privateKey);
  client.write("public key generated");

  client.write(privateKey,32);
  client.write(publicKey,32);

  String message = "";
  uint8_t signature[64];
 
  Ed25519::sign(signature, privateKey, publicKey, message.c_str(), message.length());
  
  client.write(signature,64);
}
void loop()
{
}
