#include "Curve25519.h"
#include "keys.hpp"
namespace	encrypt
{
  keys::keys()
  {
    RNG.begin("Arduino Encrypter");
  }

  void keys::load()
  {
    for (int i = 0 ; i < KeySize; i++ )
    {
      _privateKey[i] = EEPROM.read(persistent::PrivateKey + i);
    }
    
    for (int i = 0 ; i < KeySize; i++ )
    {
      _publicKey[i] = EEPROM.read(persistent::PublicKey + i);
    }
    
  }
  
  void keys::save()
  {
    for (int i = 0 ; i < KeySize; i++ )
    {
      EEPROM.update(persistent::PrivateKey + i,_privateKey[i]);
    }
    
    for (int i = 0 ; i < KeySize; i++ )
    {
      EEPROM.update(persistent::PublicKey + i,_publicKey[i]);
    }
    EEPROM.commit();
  }
  
  void keys::renew()
  {
    Ed25519::generatePrivateKey(_privateKey);
    Ed25519::derivePublicKey(_publicKey,_privateKey);
  }

  void keys::init(TransistorNoiseSource noise)
  {
    RNG.addNoiseSource(noise);
    load();
    uint8_t publicKey[32];
    Ed25519::derivePublicKey(publicKey, _privateKey);
    if (memcmp(publicKey, _publicKey, 32))
    {
        renew();
        save();
    }
  }
  
  void keys::agree(WiFiClient &client)
  {  
    uint8_t message[64];
    uint8_t signature[SignSize];
    uint8_t dhF[KeySize];
    uint8_t dhK[KeySize];

    Base64.encode((char*)message, (char*)_publicKey, KeySize);
    client.write((char*)message);

    while(!client.available()){};
    if (client.readString() == String("refuse"))
    { 
      return;// to do abort
    }

    Curve25519::dh1(dhK, dhF);
   

    Ed25519::sign
      (
      signature,
      _privateKey, 
      _publicKey, 
      message, 
      Base64.encode((char*)message, (char*)dhK, KeySize)
      );

    client.write((char*)message);
    client.write(signature, SignSize);
    
    while(KeySizeBase64 > client.available()){};
    for (uint8_t i = 0; i < KeySizeBase64 ; i++)
    {
      message [i] = client.read();
    }

    while(SignSize > client.available()){};
    for (uint8_t i = 0; i < SignSize ; i++)
    {
      signature [i] = client.read();
    }

    bool valid = Ed25519::verify(signature, _serverKey , message, KeySizeBase64);
    if (!valid)
    {
      client.write("You are a liar");
      return;// to do abort
    }
    
    Base64.decode((char*)dhK, (char*)message , KeySizeBase64);

    Curve25519::dh2(dhK, dhF);
    memcpy(_sharedKey,dhK,KeySize);
    memset(dhK,0,32);

    client.write(_sharedKey, KeySize);
  }
  void keys::askServerPub(WiFiClient &client)
  {
    // uint8_t message[64];
    // client.write("Please can you send your public key");

    // while(!client.available()){};
    // memcpy((char*)message,client.readString().c_str(),KeySizeBase64);
    // Base64.decode((char*)_serverKey, (char*)message , KeySizeBase64);
    Base64.decode((char*)_serverKey, "GskUXMWVAz26Ko+AAywcB75HGZurjfnznrgsxaSY1mc=", KeySizeBase64);
  }
}