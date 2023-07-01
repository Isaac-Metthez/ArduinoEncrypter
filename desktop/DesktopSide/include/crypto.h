#pragma once

#include <iostream>
#include <sstream>
#include <filesystem>
#include <fstream>
#include <ctime>
#include <iomanip>

#pragma warning(push)
#pragma warning(disable : 26495 26439 26451)

#include "xed25519.h"
#include "cryptlib.h"
#include "osrng.h"
#include "files.h"
#include "filters.h"
#include "modes.h"
#include "hex.h"
#include "base64.h"
#include "rijndael.h"

#pragma warning(pop)

namespace encryption {
	//CHANGE ON RELEASE
	constexpr auto PRIVATE_KEY_FILE = ".\\authKeys\\key";
	constexpr auto PUBLIC_KEY_FILE = ".\\authKeys\\key.pub";
	constexpr auto CLIENTS_KEYS_PATH = ".\\knownClients";
	
	constexpr auto AUTHENTICATION_HEADER = "AuPK";
	constexpr auto AGREEMENT_HEADER = "DHPK";
	constexpr auto DATA_HEADER = "Data";
	constexpr auto ED25519_PRK_HEX_HEADER = "302e020100300506032b657004220420";
	constexpr auto ED25519_PUK_HEX_HEADER = "302a300506032b6570032100";
	constexpr auto B64_KEY_LENGTH = 44;
	constexpr auto SIGNATURE_LENGTH = 64;
	constexpr auto BOOL_SIZE = 8;
	constexpr auto INT_SIZE = 4;

	class Crypto
	{
		private:
			CryptoPP::ed25519::Signer serverSigner;
			CryptoPP::ed25519::Verifier serverVerifier;
			CryptoPP::ed25519::Verifier clientVerifier;
			CryptoPP::x25519 ecdh;
			CryptoPP::SecByteBlock sharedSecret;
			CryptoPP::SecByteBlock serverIv;
			CryptoPP::SecByteBlock clientIv;
			
			std::string authMessage;
			std::string dataMessage;
			int nbClients;

			template <typename Encoder>
			std::string encode(std::string plainStr);
			template <typename Decoder>
			std::string decode(std::string encodedStr);
		public:
			Crypto();
			void loadAuthKeys();
			bool checkForKnownKey(std::string rawKey);
			void saveClientPublicKey();
			template <typename KeyType>
			void loadPublicKeyFromData(std::string rawKey, KeyType& key);
			std::string sign(std::string plainStr);
			bool verifySignature(std::string authMessage, std::string signature);
			void makeAgreement(std::string rawKey);
			void encrypt(char* plainStr, int size);
			void decrypt(char* cipher, int size);
			void setIv(CryptoPP::SecByteBlock& iv);
			void setIv(CryptoPP::SecByteBlock& iv, std::string );
			std::string getAuthMessage();
			std::string getDataMessage();
	};

	template<typename KeyType>
	inline void Crypto::loadPublicKeyFromData(std::string rawKey, KeyType& key) {
		CryptoPP::AutoSeededRandomPool prng;

		rawKey = decode<CryptoPP::Base64Decoder>(rawKey);

		rawKey = (typeid(KeyType) == typeid(CryptoPP::ed25519PublicKey) ? ED25519_PUK_HEX_HEADER : "") + encode<CryptoPP::HexEncoder>(rawKey);

		rawKey = decode<CryptoPP::HexDecoder>(rawKey);

		CryptoPP::StringSource src(rawKey, true);

		key.Load(src);
		if (!key.Validate(prng, 3))
			throw std::runtime_error("Invalid key");
	}

	template<typename Encoder>
	inline std::string Crypto::encode(std::string plainStr) {
		std::string encodedStr;
		CryptoPP::StringSource(plainStr, true, new Encoder(new CryptoPP::StringSink(encodedStr), false));
		return encodedStr;
	}

	template<typename Decoder>
	inline std::string Crypto::decode(std::string encodedStr) {
		std::string decodedStr;
		CryptoPP::StringSource(encodedStr, true, new Decoder(new CryptoPP::StringSink(decodedStr)));
		return decodedStr;
	}
};