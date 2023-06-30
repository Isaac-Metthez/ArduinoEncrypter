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
#include "hex.h"
#include "base64.h"

#pragma warning(pop)

#pragma once
namespace Encryption {
	#define AUTH_KEYS_PATH ".\\authKeys\\"
	#define	PRIVATE_KEY_FILE "key"
	#define PUBLIC_KEY_FILE "key.pub"

	#define CLIENTS_KEYS_PATH ".\\knownClients\\"
	
	#define ED25519_PRK_HEX_HEADER "302e020100300506032b657004220420"
	#define ED25519_PUK_HEX_HEADER "302a300506032b6570032100"

	#define BOOL_SIZE 8
	#define INT_SIZE 4

	class Crypto
	{
		private:
			static CryptoPP::ed25519::Signer serverSigner;
			static CryptoPP::ed25519::Verifier serverVerifier;
			static CryptoPP::ed25519::Verifier clientVerifier;
			static CryptoPP::x25519 ecdh;
			static CryptoPP::SecByteBlock sharedSecret;
			static int nbClients;
			static unsigned int serverSequenceNumber0;
			static unsigned int serverSequenceNumber1;
			static unsigned int serverSequenceNumber2;
			static unsigned int clientSequenceNumber0;
			static unsigned int clientSequenceNumber1;
			static unsigned int clientSequenceNumber2;
			static std::string authMessage;
			static std::string dataMessage;

			template <typename Encoder>
			static std::string encode(std::string plainStr);
			template <typename Decoder>
			static std::string decode(std::string encodedStr);
		public:
			static void loadAuthKeys();
			static void test(std::string testStr);
			static bool checkForKnownKey(std::string rawKey);
			static void saveClientPublicKey();
			template <typename KeyType>
			static void loadPublicKeyFromData(std::string rawKey, KeyType& key);
			static std::string sign(std::string plainStr);
			static bool verifySignature(std::string authMessage, std::string signature);
			static void makeAgreement(std::string rawKey);
			static std::string getAuthMessage();
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