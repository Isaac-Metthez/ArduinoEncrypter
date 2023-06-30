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
	FileSource fs1("private.key.bin", true);
	ed25519::Signer signer;
	signer.AccessPrivateKey().Load(fs1);
	if(!signer.GetPrivateKey().Validate(prng, 3))
		throw std::runtime_error("Invalid private key");

	const ed25519PrivateKey& prvKey = dynamic_cast<const ed25519PrivateKey&>(signer.GetPrivateKey());

    std::string encodedPrivateKey;
    prvKey.Save(HexEncoder(new StringSink(encodedPrivateKey)).Ref());
    std::cout << "Private Key (Hex):\t" << encodedPrivateKey << std::endl << std::endl;



	//PUK

	FileSource fs2("public.key.bin", true);
	ed25519::Verifier verifier(signer);
	verifier.AccessPublicKey().Load(fs2);
	if(!verifier.GetPublicKey().Validate(prng, 3))
		throw std::runtime_error("Invalid public key");
	
	const ed25519PublicKey& pubKey = dynamic_cast<const ed25519PublicKey&>(verifier.GetPublicKey());

    std::string encodedPublicKey;
    // pubKey.BEREncode(StringSink(encodedPublicKey).Ref());
    // std::cout << "Public Key (Raw) : " << encodedPublicKey << std::endl << std::endl;
    pubKey.Save(HexEncoder(new StringSink(encodedPublicKey)).Ref());
    std::cout << "Public Key (Hex):\t" << encodedPublicKey << std::endl << std::endl;

	// std::string pukStr = "302A300506032B65700321008612FEDD71A4C190ADD5FC65EFED7F512E584EB353A971E8F7589AEA84B31FF2";

	// std::string decodedPublicKey;
    // StringSource(encodedPublicKey, true,
    //     new HexDecoder(new StringSink(decodedPublicKey)));

    // std::cout << "Decoded Public Key: " << decodedPublicKey << std::endl << std::endl;

	// StringSource src(decodedPublicKey, true);

	// ed25519PublicKey puk;
	// puk.BERDecode(src);
	// if(!puk.Validate(prng, 3))
	// 	throw std::runtime_error("Invalid public key");
}
