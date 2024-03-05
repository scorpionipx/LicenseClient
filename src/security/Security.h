#pragma once

#include <iostream>
#include <string>
#include <sstream>
#include <locale>
#include <codecvt>

#define cimg_display 0
#define cimg_use_png 1
#include <CImg.h>

#include <cryptopp/aes.h>
#include <cryptopp/base64.h>
#include <cryptopp/cryptlib.h>
#include <cryptopp/dh.h>
#include <cryptopp/dh2.h>
#include <cryptopp/eccrypto.h>
#include <cryptopp/hex.h>
#include <cryptopp/files.h>
#include <cryptopp/filters.h>
#include <cryptopp/modes.h>
#include <cryptopp/osrng.h>
#include <cryptopp/oids.h>
#include <cryptopp/rsa.h>
#include <cryptopp/secblock.h>

#include <nlohmann/json.hpp>

#include "..\utils\utils.h"

using namespace CryptoPP;
using json = nlohmann::json;

class Security {
public:
	// Constructor
	Security(const std::string& keyStorage);
	// Functions
	std::string AESDecrypt(const std::string& ciphertext, const std::string& aesKey = "", const std::string& iv = "");
	std::string AESEncrypt(const std::string& plaintext, const std::string& aesKey = "", const std::string& iv = "");
	std::string RSAEncrypt(const std::string& plainText);
	void setOwnerRSAPublicKey(const std::string& ownerPublicKey);
	std::string getAESKey();
private:
	AutoSeededRandomPool rng;
	RSA::PublicKey ownerRSAPublicKey;

	SecByteBlock AESkey;
	byte initVector[AES::BLOCKSIZE];

	// Functions
	void generateAESKey();
	void loadAESKey();

	// Poor Stenography
	std::string keyStorage;
	void setKey(const std::string& text);
	std::string getKey();
};

