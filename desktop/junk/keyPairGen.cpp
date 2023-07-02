#include <cryptopp/osrng.h>
#include <cryptopp/xed25519.h>
#include <cryptopp/files.h>

#include <cryptopp/filters.h>
#include <cryptopp/base64.h>
#include <cryptopp/hex.h>

using namespace CryptoPP;

int main(){
	AutoSeededRandomPool prng;
	
	//PRK

	ed25519::Signer signer;
	signer.AccessPrivateKey().GenerateRandom(prng);

	FileSink fs1("private.key.bin");
	signer.GetPrivateKey().Save(fs1);

    std::string hexPrivateKey;
    signer.GetPrivateKey().Save(HexEncoder(new StringSink(hexPrivateKey)).Ref());
    std::cout << "Private Key (Hex):\t" << hexPrivateKey << std::endl << std::endl;


	//PUK

	ed25519::Verifier verifier(signer);
	
	FileSink fs2("public.key.bin");
	verifier.GetPublicKey().Save(fs2);
	
    std::string hexPublicKey;
    verifier.GetPublicKey().Save(HexEncoder(new StringSink(hexPublicKey)).Ref());
    std::cout << "Public Key (Hex):\t" << hexPublicKey << std::endl << std::endl;
}
