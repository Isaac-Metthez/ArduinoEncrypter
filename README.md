# ArduinoEncrypter
Encrypt data between arduino and C++ interface
## References
- [gnupg2 git](https://github.com/gpg/gnupg)
- [arduino crypto reference](https://rweather.github.io/arduinolibs/crypto.html)
- [wikipedia EdDSA (signature)](https://en.wikipedia.org/wiki/EdDSA)
- [wikipwdia Curve25519 (shared key)](https://en.wikipedia.org/wiki/Curve25519)
- [wikipedia encryption mode](https://en.wikipedia.org/wiki/Block_cipher_mode_of_operation)

## Tasks

- [ ] [Document] How we will proceed
- [ ] [Document] Search a way to use gpg in a Cpp application control with implementation test
- [ ] [Document] Search a way to use gpg with an arduino control with implementation test
- [ ] [Document] Use wifi with Arduino and then connect arduino with TCP to PC with test implementation
- [ ] [Code] PC connection with TCP to Arduino
- [ ] [Code] Arduino connection with TCP to PC
- [ ] [Code] Arduino create gpg key and save it into EEPROM 
- [ ] [Code] PC create symetric key
- [ ] [Code] Dialog between Arduino and PC to exchange symetric key
- [ ] [Code] Arduino encrypt with sym key (must use smth as "Mode Cipher Block Chaining")
- [ ] [Code] PC decrypt 
