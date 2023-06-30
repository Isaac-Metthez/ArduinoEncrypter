#include "../include/crypto.h"

namespace Encryption {

	CryptoPP::ed25519::Signer Crypto::serverSigner;
	CryptoPP::ed25519::Verifier Crypto::serverVerifier;
	CryptoPP::ed25519::Verifier Crypto::clientVerifier;
	CryptoPP::x25519 Crypto::ecdh;
	CryptoPP::SecByteBlock Crypto::sharedSecret;
	int Crypto::nbClients;
	unsigned int Crypto::serverSequenceNumber0 = 0;
	unsigned int Crypto::serverSequenceNumber1 = 0;
	unsigned int Crypto::serverSequenceNumber2 = 0;
	unsigned int Crypto::clientSequenceNumber0 = 0;
	unsigned int Crypto::clientSequenceNumber1 = 0;
	unsigned int Crypto::clientSequenceNumber2 = 0;

	std::string Crypto::authMessage;
	std::string Crypto::dataMessage;

	void Crypto::test(std::string testStr) {
	
	}

	void Crypto::loadAuthKeys() {

		CryptoPP::AutoSeededRandomPool prng;

		//PRK
		std::ifstream prkFile(AUTH_KEYS_PATH PRIVATE_KEY_FILE);
		if(prkFile.good())
			serverSigner.AccessPrivateKey().Load(CryptoPP::FileSource(AUTH_KEYS_PATH PRIVATE_KEY_FILE, true).Ref());
		else {
			serverSigner.AccessPrivateKey().GenerateRandom(prng);

			CryptoPP::FileSink fsPRK(AUTH_KEYS_PATH PRIVATE_KEY_FILE);
			serverSigner.GetPrivateKey().Save(fsPRK);
		}
		std::string privateKey;
		serverSigner.GetPrivateKey().Save(CryptoPP::StringSink(privateKey).Ref());
		std::string encodedPrivateKey;
		CryptoPP::StringSource(privateKey, true, new CryptoPP::Base64Encoder(new CryptoPP::StringSink(encodedPrivateKey), false));
		std::cout << "Private Key:\t" << encodedPrivateKey << std::endl << std::endl;

		//PUK
		std::ifstream pukFile(AUTH_KEYS_PATH PUBLIC_KEY_FILE);
		if(pukFile.good())
			serverVerifier.AccessPublicKey().Load(CryptoPP::FileSource(AUTH_KEYS_PATH PUBLIC_KEY_FILE, true).Ref());
		else {
			serverVerifier = CryptoPP::ed25519::Verifier(serverSigner);
			CryptoPP::FileSink fsPUK(AUTH_KEYS_PATH PUBLIC_KEY_FILE);
			serverVerifier.GetPublicKey().Save(fsPUK);
		}
		std::string publicKey;
		serverVerifier.GetPublicKey().Save(CryptoPP::StringSink(publicKey).Ref());
		std::string encodedPublicKey;
		encodedPublicKey = encode<CryptoPP::Base64Encoder>(publicKey);
		std::cout << "Public Key:\t" << encodedPublicKey << std::endl << std::endl;
		authMessage += "AuPK" + encodedPublicKey.substr(16, 44);
	}

	bool Crypto::checkForKnownKey(std::string rawKey) {
		CryptoPP::AutoSeededRandomPool prng;
		CryptoPP::ed25519PublicKey clientKey;
		loadPublicKeyFromData<CryptoPP::ed25519PublicKey>(rawKey, clientKey);
		if (!clientKey.Validate(prng, 3))
			throw std::runtime_error("Invalid public key");

		nbClients = 0;
		for (const auto& entry : std::filesystem::directory_iterator(CLIENTS_KEYS_PATH)) {
			nbClients++;
			std::string keyPath = entry.path().string();
			CryptoPP::ed25519PublicKey key;
			key.Load(CryptoPP::FileSource(keyPath.c_str(), true).Ref());
			if (key.GetPublicElement() == clientKey.GetPublicElement()) {
				std::cout << "Client is already authenticated." << std::endl;
				clientVerifier.AccessPublicKey().Load(CryptoPP::FileSource(keyPath.c_str(), true).Ref());
				return true;
			}
		}
		loadPublicKeyFromData<CryptoPP::ed25519PublicKey>(rawKey, dynamic_cast<CryptoPP::ed25519PublicKey&>(clientVerifier.AccessPublicKey()));
		return false;
	}

	void Crypto::saveClientPublicKey() {
		CryptoPP::FileSink fs((std::string(CLIENTS_KEYS_PATH) + "client_" + std::to_string(++nbClients) + ".pub").c_str());
		clientVerifier.GetPublicKey().Save(fs);
	}

	std::string Crypto::sign(std::string plainStr) {
		CryptoPP::AutoSeededRandomPool prng;
		std::string signature;

		CryptoPP::StringSource(plainStr, true, new CryptoPP::SignerFilter(prng, serverSigner, new CryptoPP::StringSink(signature)));

		std::cout << "Signature: " << encode<CryptoPP::HexEncoder>(signature) << "\n" << std::endl;
		//signature = encode<CryptoPP::HexEncoder>(signature);
		return signature;
	}

	bool Crypto::verifySignature(std::string authMessage, std::string signature) {
		CryptoPP::AutoSeededRandomPool prng;
		bool result = false;
		//signature = decode<CryptoPP::HexDecoder>(signature); //COMMENT
		CryptoPP::StringSource(signature + authMessage, true, new CryptoPP::SignatureVerificationFilter(clientVerifier, new CryptoPP::ArraySink((CryptoPP::byte*)&result, sizeof(result))));

		std::cout << "Signature on message verified: " << std::boolalpha << result << "\n" << std::endl;
		return result;
	}

	void Crypto::makeAgreement(std::string rawKey) {
		CryptoPP::AutoSeededRandomPool prng;
		ecdh = CryptoPP::x25519(prng);

		CryptoPP::SecByteBlock prk(ecdh.PrivateKeyLength());
		CryptoPP::SecByteBlock puk(ecdh.PublicKeyLength());
		ecdh.GenerateKeyPair(prng, prk, puk);

		std::string decodedKey = decode<CryptoPP::Base64Decoder>(rawKey);

		CryptoPP::SecByteBlock clientPuk(decodedKey.size());
		std::memcpy(clientPuk.data(), decodedKey.data(), clientPuk.size());

		sharedSecret.resize(ecdh.AgreedValueLength());

		if (!ecdh.Agree(sharedSecret, prk, clientPuk))
			throw std::runtime_error("Failed to reach shared secret");

		std::string secretStr, prkStr, pukStr;
		CryptoPP::StringSource(sharedSecret, sharedSecret.size(), true, new CryptoPP::Base64Encoder(new CryptoPP::StringSink(secretStr), false));
		CryptoPP::StringSource(puk, puk.size(), true, new CryptoPP::Base64Encoder(new CryptoPP::StringSink(pukStr), false));
		CryptoPP::StringSource(prk, prk.size(), true, new CryptoPP::Base64Encoder(new CryptoPP::StringSink(prkStr), false));

		std::cout << "PRK: " << prkStr << std::endl;
		std::cout << "PUK: " << pukStr << std::endl;
		std::cout << "Shared secret: " << secretStr << std::endl;
		authMessage += "DHPK" + pukStr;

	}
	std::string Crypto::getAuthMessage()
	{
		authMessage += sign(authMessage);
		return authMessage;
	}
}