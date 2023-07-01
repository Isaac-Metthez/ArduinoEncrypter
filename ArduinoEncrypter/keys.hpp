#ifndef KEYS
#define KEYS
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
  constexpr auto ServerKey = "9CSaItSgN3QJS0jHGy4EuJbD5VuP72JE+apdzfVsu/8=";
  constexpr auto KeySize = 32;
  constexpr auto KeySizeBase64 = 44;
  constexpr auto SignSize = 64;
  constexpr auto SignSizeInUint = 16;
  constexpr auto AuPK = "AuPK";
  constexpr auto DHPK = "DHPK";
  constexpr auto HeaderSize = 4;
  constexpr auto HeaderAndOneKeySize = HeaderSize + KeySizeBase64;
  constexpr auto AuthenticateMsgWithoutSignSize = 96;
  constexpr auto AuthenticateMsgSize = AuthenticateMsgWithoutSignSize+SignSize;

  class Keys
  {
  protected:
    void save();
    void load();
    void renew();

    uint8_t _privateKey[KeySize];
    uint8_t _publicKey[KeySize];
    uint8_t _serverKey[KeySize];
    uint8_t _sharedKey[KeySize];

    bool internalAgree(WiFiClient &client);
  public:
    Keys();

    void init(TransistorNoiseSource noise);
    void askServerPub(WiFiClient &client);
    void agree(WiFiClient &client);

    uint8_t *getPublicKey() {return _publicKey;};
    uint8_t *getPrivateKey() {return _privateKey;};
    uint8_t *getServerKey() {return _serverKey;};
    uint8_t *getSharedKey() {return _sharedKey;};
  };
}
#endif