#include <stdint.h>
#ifndef KEYS_hpp
#define KEYS_hpp
#include <RNG.h>
#include <WiFiNINA.h>
#include <Curve25519.h>
#include <Ed25519.h>
#include <utility/ProgMemUtil.h>
#include <TransistorNoiseSource.h>
#include <string.h>
#include <FlashAsEEPROM.h>
#include <Base64.h>
#include "persistent.hpp"

namespace	encrypt
{
  constexpr auto KeySize = 32;
  constexpr auto KeySizeBase64 = 44;
  constexpr auto SignSize = 64;
  class keys
  {
  protected:
    void save();
    void load();
    void renew();

    uint8_t _privateKey[KeySize];
    uint8_t _publicKey[KeySize];
    uint8_t _serverKey[KeySize];
    uint8_t _sharedKey[KeySize];
  public:
    keys();
    ~keys() = default;

    void init(TransistorNoiseSource noise);
    void askServerPub(WiFiClient &client);
    void agree(WiFiClient &client);

    uint8_t *getPrivateKey() {return _privateKey;};
    uint8_t *getPublicKey() {return _publicKey;};
    uint8_t *getSharedKey() {return _sharedKey;};
  };
}
#endif