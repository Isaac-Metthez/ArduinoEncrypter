#include "keys.hpp"
namespace	encrypt
{
  Keys::Keys()
  {
    RNG.begin("Arduino Encrypter");
  }

  void Keys::load()
  {
    for (int i = 0 ; i < KeySize; i++ )
    {
      _privateKey[i] = EEPROM.read(persistent::PrivateKey + i);
    }
    
    for (int i = 0 ; i < KeySize; i++ )
    {
      _publicKey[i] = EEPROM.read(persistent::PublicKey + i);
    }
    
    for (int i = 0 ; i < KeySize; i++ )
    {
      _serverKey[i] = EEPROM.read(persistent::ServerKey + i);
    }

  }
  
  void Keys::save()
  {
    for (int i = 0 ; i < KeySize; i++ )
    {
      EEPROM.update(persistent::PrivateKey + i,_privateKey[i]);
    }
    
    for (int i = 0 ; i < KeySize; i++ )
    {
      EEPROM.update(persistent::PublicKey + i,_publicKey[i]);
    }
    
    for (int i = 0 ; i < KeySize; i++ )
    {
      EEPROM.update(persistent::ServerKey + i,_serverKey[i]);
    }
    
    EEPROM.commit();
  }
  
  void Keys::renew()
  {
    Ed25519::generatePrivateKey(_privateKey);
    Ed25519::derivePublicKey(_publicKey,_privateKey);
  }

  void Keys::init(TransistorNoiseSource noise)
  {
    RNG.addNoiseSource(noise);
    load();
    uint8_t publicKey[32];
    Ed25519::derivePublicKey(publicKey, _privateKey);
    if (memcmp(publicKey, _publicKey, 32))
    {
        Base64.decode((char*)_serverKey, ServerKey, KeySizeBase64);
        renew();
        save();
    }
  }
  

  void Keys::agree(WiFiClient &client)
  {
    uint wait = 2048;
    while(!internalAgree(client))
    {
      delay(wait);
      if (wait << 1 > wait )
        wait <<= 1;
    }
  }

  bool Keys::internalAgree(WiFiClient &client)
  {  
    uint8_t message[AuthenticateMsgSize];
    uint8_t dhF[KeySize];
    uint8_t dhK[KeySize];

    for (uint8_t i = 0; i < HeaderSize ; i++)
      message[i] = AuPK[i];
    Base64.encode((char*)message + HeaderSize, (char*)_publicKey, KeySize);

    Curve25519::dh1(dhK, dhF);
    for (uint8_t i = 0; i < HeaderSize ; i++)
      message[HeaderAndOneKeySize+ i] = DHPK[i];
    Base64.encode((char*)message + HeaderSize + HeaderAndOneKeySize, (char*)dhK, KeySize);

    Ed25519::sign(
      message + AuthenticateMsgWithoutSignSize,
      _privateKey, 
      _publicKey, 
      message, 
      AuthenticateMsgWithoutSignSize);

    client.write((uint8_t*)message, AuthenticateMsgSize);

    while(AuthenticateMsgSize > client.available()){};
    for (uint8_t i = 0; i < AuthenticateMsgSize ; i++)
    {
      message[i] = client.read();
    }

    if (!Ed25519::verify(
          message + AuthenticateMsgWithoutSignSize, 
          _serverKey, 
          message, 
          AuthenticateMsgWithoutSignSize))
    {
      return false;
    }
    
    Base64.decode((char*)dhK, (char*)message + HeaderSize + HeaderAndOneKeySize, KeySizeBase64);
    
    Curve25519::dh2(dhK, dhF);

    memcpy(_sharedKey,dhK,KeySize);
    memset(dhK,0xFF,KeySize);   

    return true;
  }

  void Keys::askServerPub(WiFiClient &client)
  {
    // uint8_t message[64];
    // client.write("Please can you send your public key");

    // while(!client.available()){};
    // memcpy((char*)message,client.readString().c_str(),KeySizeBase64);
    // Base64.decode((char*)_serverKey, (char*)message , KeySizeBase64);
    // Base64.decode((char*)_serverKey, "GskUXMWVAz26Ko+AAywcB75HGZurjfnznrgsxaSY1mc=", KeySizeBase64);
  }
}