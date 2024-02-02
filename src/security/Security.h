#pragma once

#include <iostream>
#include <string>
#include <sstream>
#include <windows.h>
#include <wincred.h>
#include <wincrypt.h>
#include <locale>
#include <codecvt>

#include <cryptopp/aes.h>
#include <cryptopp/base64.h>
#include <cryptopp/cryptlib.h>
#include <cryptopp/hex.h>
#include <cryptopp/files.h>
#include <cryptopp/filters.h>
#include <cryptopp/modes.h>
#include <cryptopp/osrng.h>
#include <cryptopp/rsa.h>
#include <nlohmann/json.hpp>
#include <opencv2/opencv.hpp>

#include "..\utils\utils.h"

using namespace CryptoPP;
using namespace cv;
using json = nlohmann::json;

class Security {
public:
	// Constructor
	Security();
	// Functions
	std::string AESDecrypt(const std::string& ciphertext);
	std::string AESEncrypt(const std::string& plaintext);
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
	const std::string imageFilepath = "d:/asammdf.png";
	void setKey(const std::string& text);
	std::string getKey();
};

