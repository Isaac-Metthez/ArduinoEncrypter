# ArduinoEncrypter
Encrypt data between arduino and C++ interface
## References
- [gnupg2 git](https://github.com/gpg/gnupg)
- [arduino crypto reference](https://rweather.github.io/arduinolibs/crypto.html)
- [wikipedia EdDSA (signature)](https://en.wikipedia.org/wiki/EdDSA)
- [wikipwdia Curve25519 (shared key)](https://en.wikipedia.org/wiki/Curve25519)
- [wikipedia encryption mode](https://en.wikipedia.org/wiki/Block_cipher_mode_of_operation)

## Tasks

- [x] [Document] How we will proceed
- [x] [Document] Search a way to use gpg in a Cpp application control with implementation test
- [x] [Document] Search a way to use gpg with an arduino control with implementation test
- [x] [Document] Use wifi with Arduino and then connect arduino with TCP to PC with test implementation
- [x] [Code] PC connection with TCP to Arduino
- [x] [Code] Arduino connection with TCP to PC
- [x] [Code] Arduino create gpg key and save it into EEPROM 
- [x] [Code] PC create symetric key
- [x] [Code] Dialog between Arduino and PC to exchange symetric key
- [x] [Code] Arduino encrypt with sym key (must use smth as "Mode Cipher Block Chaining")
- [x] [Code] PC decrypt 
