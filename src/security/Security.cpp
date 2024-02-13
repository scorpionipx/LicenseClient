#include "pch.h"

#include "Security.h"


Security::Security(const std::string& keyStorage) {
    this->keyStorage = keyStorage;

    // Initialize Crypto++ library
    CryptoPP::AutoSeededRandomPool rng;
    AESkey.resize(AES::DEFAULT_KEYLENGTH);   

    if (getKey() == "") {
        std::cout << "GenerateKey called.." << std::endl;
        // Generate AES Key
        generateAESKey();

        // Store Key
        std::string _aesKey = getAESKey();
        setKey(_aesKey);
    }
    else {
        // Load Key
        std::cout << "LoadKey called.." << std::endl;
        loadAESKey();
    }
}

void Security::generateAESKey() {
    // Generate AES Key
    rng.GenerateBlock(AESkey, AESkey.size());

    // Generate a random IV
    rng.GenerateBlock(initVector, sizeof(initVector));
}

std::string Security::AESDecrypt(const std::string& ciphertext) {
    std::string decrypted;

    CBC_Mode<AES>::Decryption decryption(AESkey, AESkey.size(), initVector);
    StringSource(
        ciphertext,
        true,
        new StreamTransformationFilter(decryption, new StringSink(decrypted))
    );

    return decrypted;
}

std::string Security::AESEncrypt(const std::string& plaintext) {
    std::string ciphertext;

    CBC_Mode<AES>::Encryption encryption(AESkey, AESkey.size(), initVector);
    StringSource(
        plaintext,
        true,
        new StreamTransformationFilter(encryption, new StringSink(ciphertext)));

    return ciphertext;
}

std::string Security::RSAEncrypt(const std::string& plainText) {
    AutoSeededRandomPool rng;

    // Create Encryptor
    RSAES_OAEP_SHA_Encryptor encryptor(ownerRSAPublicKey);

    // Encrypt
    std::string cipherText; 
    StringSource(
        plainText, 
        true,
        new PK_EncryptorFilter(rng, encryptor, new StringSink(cipherText))
    );

    return cipherText;
}

void Security::setOwnerRSAPublicKey(const std::string& ownerPublicKey) {
    // Convert from Hex to String
    std::string ownerPublicKey_str = hexToString(ownerPublicKey);
    // Store Key
    StringSource keySource(ownerPublicKey_str, true);
    this->ownerRSAPublicKey.Load(keySource);
}

std::string Security::getAESKey() {
    json data;

    // Save Key in HEX Format
    std::string AESKeyStr = stringToHex(std::string(reinterpret_cast<const char*>(AESkey.data()), AESkey.size()));
    std::string initVectorStr = stringToHex(std::string(reinterpret_cast<const char*>(initVector), sizeof(initVector)));

    data["AESKey"] = AESKeyStr;
    data["InitVector"] = initVectorStr;

    return data.dump();
}

void Security::loadAESKey() {
    json data = json::parse(getKey());

    std::string _aesKey = hexToString(data["AESKey"]);
    std::string _initVector = hexToString(data["InitVector"]);

    memcpy(AESkey, _aesKey.data(), _aesKey.size());
    memcpy(initVector, _initVector.data(), _initVector.size());
}

void Security::setKey(const std::string& text) {

    // Load input image
    cimg_library::CImg<unsigned char> image;
    image.load_png(keyStorage.c_str());
    
    // Calculate message length and make sure it fits in the image
    size_t messageLength = text.length();
    if (messageLength * 8 > image.width() * image.height() * image.spectrum()) {
        std::cerr << "Key store failed.\n";
        return;
    }

    size_t bitIndex = 0;

    // Hide message in image data
    cimg_forXY(image, x, y) {
        cimg_forC(image, rgb) {
            if (bitIndex <= messageLength * 8) {
                unsigned char& pixel = image(x, y, 0, rgb);
                // Clear LSB Bit
                // pixel = (pixel & ~1)
                // Select char from text.
                // text[bitIndex / 8]
                // Select bit from selected char
                // bitIndex % 8
                // Set LSB if needed.
                pixel = (pixel & ~1) | ((text[bitIndex / 8] >> (bitIndex % 8)) & 0x01);
                // Go to next bit.
                ++bitIndex;
            }
        }
    }

    // Save modified image
    image.save(keyStorage.c_str());
}

std::string Security::getKey() {

    // Load input image
    cimg_library::CImg<unsigned char> image;
    image.load_png(keyStorage.c_str());

    std::string decodedText;
    char ch = 0;
    size_t bitIndex = 0;

    cimg_forXY(image, x, y) {
        cimg_forC(image, rgb) {
            // Extract bit
            // (image(x, y, 0, rgb) & 1)
            // Put in position
            ch += (image(x, y, 0, rgb) & 0x01) << (bitIndex % 8);
            ++bitIndex;

            if (bitIndex % 8 == 0) {
                decodedText += ch;
                ch = 0;
            }

            // Magic Number
            if (decodedText.length() >= 93) {
                // Message decoded completely
                try {
                    json _don_t_care = json::parse(decodedText);
                    return decodedText;
                }
                catch (const nlohmann::json::exception) {
                    return "";
                }
            }
        }
    }
    return "";
}
