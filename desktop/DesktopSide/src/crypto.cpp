#include "../include/crypto.h"

namespace encryption {
	Crypto::Crypto() {
		loadAuthKeys();
		setIv(serverIv);
		setIv(clientIv);
	}

	void Crypto::loadAuthKeys() {
		CryptoPP::AutoSeededRandomPool prng;
		//PRK
		std::ifstream prkFile(PRIVATE_KEY_FILE);
		if(prkFile.good())
			serverSigner.AccessPrivateKey().Load(CryptoPP::FileSource(PRIVATE_KEY_FILE, true).Ref());
		else {
			serverSigner.AccessPrivateKey().GenerateRandom(prng);

			CryptoPP::FileSink fsPRK(PRIVATE_KEY_FILE);
			serverSigner.GetPrivateKey().Save(fsPRK);
		}
		std::string privateKey;
		serverSigner.GetPrivateKey().Save(CryptoPP::StringSink(privateKey).Ref());
		std::string encodedPrivateKey;
		CryptoPP::StringSource(privateKey, true, new CryptoPP::Base64Encoder(new CryptoPP::StringSink(encodedPrivateKey), false));
		std::cout << "Private Key:\t" << encodedPrivateKey << std::endl << std::endl;

		//PUK
		std::ifstream pukFile(PUBLIC_KEY_FILE);
		if(pukFile.good())
			serverVerifier.AccessPublicKey().Load(CryptoPP::FileSource(PUBLIC_KEY_FILE, true).Ref());
		else {
			serverVerifier = CryptoPP::ed25519::Verifier(serverSigner);
			CryptoPP::FileSink fsPUK(PUBLIC_KEY_FILE);
			serverVerifier.GetPublicKey().Save(fsPUK);
		}
		std::string publicKey;
		serverVerifier.GetPublicKey().Save(CryptoPP::StringSink(publicKey).Ref());
		std::string encodedPublicKey;
		encodedPublicKey = encode<CryptoPP::Base64Encoder>(publicKey);
		std::cout << "Public Key:\t" << encodedPublicKey << std::endl << std::endl;
		authMessage += AUTHENTICATION_HEADER + encodedPublicKey.substr(16, 44);
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
		return signature;
	}

	bool Crypto::verifySignature(std::string message, std::string signature) {
		CryptoPP::AutoSeededRandomPool prng;
		bool result = false;
		CryptoPP::StringSource(signature + message, true, new CryptoPP::SignatureVerificationFilter(clientVerifier, new CryptoPP::ArraySink((CryptoPP::byte*)&result, sizeof(result))));
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
		authMessage += AGREEMENT_HEADER + pukStr;

	}

	void Crypto::encrypt(char* plain, int size) {
		unsigned char* cipher = new unsigned char[size];
		CryptoPP::CBC_Mode<CryptoPP::AES>::Encryption encryptor;
		encryptor.SetKeyWithIV(sharedSecret, sharedSecret.size(), serverIv);

		CryptoPP::ArraySource(reinterpret_cast<const CryptoPP::byte*>(plain), size, true,
			new CryptoPP::StreamTransformationFilter(encryptor,
				new CryptoPP::ArraySink(cipher, size), CryptoPP::StreamTransformationFilter::NO_PADDING));

		std::memcpy(plain, cipher, size);
		setIv(serverIv, std::string(reinterpret_cast<char*>(cipher), size));
		delete[] cipher;
	}

	void Crypto::decrypt(char* cipher, int size) {
		unsigned char* recovered = new unsigned char[size];
		CryptoPP::CBC_Mode<CryptoPP::AES>::Decryption decryptor;
		decryptor.SetKeyWithIV(sharedSecret, sharedSecret.size(), clientIv);

		CryptoPP::ArraySource(reinterpret_cast<const CryptoPP::byte*>(cipher), size, true,
			new CryptoPP::StreamTransformationFilter(decryptor,
				new CryptoPP::ArraySink(recovered, size), CryptoPP::StreamTransformationFilter::NO_PADDING));

		std::memcpy(cipher, recovered, size);
		setIv(clientIv, std::string(reinterpret_cast<char*>(cipher), size));
		delete[] recovered;
	}


	void Crypto::setIv(CryptoPP::SecByteBlock& iv) {
		iv.resize(CryptoPP::AES::BLOCKSIZE);
		std::memset(iv, 0, iv.size());
	}

	void Crypto::setIv(CryptoPP::SecByteBlock& iv, std::string ivStr) {
		std::memcpy(iv.data(), ivStr.substr(ivStr.size() - 16).data(), iv.size());
	}

	std::string Crypto::getAuthMessage()
	{
		authMessage += sign(authMessage);
		return authMessage;
	}
}