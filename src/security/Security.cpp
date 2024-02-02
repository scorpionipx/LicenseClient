#include "pch.h"

#include "Security.h"


Security::Security() {
    // Initialize Crypto++ library
    CryptoPP::AutoSeededRandomPool rng;
    AESkey.resize(AES::DEFAULT_KEYLENGTH);   

    if (getKey() == "") {
        std::cout << "GenerateKey called.." << std::endl;
        // Generate AES Key
        generateAESKey();

        // ShortTest

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

    Mat img = imread(imageFilepath);
    if (img.empty()) {
        std::cerr << "Error: Could not open or find the image!" << std::endl;
        return;
    }

    int charPositionInText = 0;
    int bitPositionInChar = 0;

    for (int row = 0; row < img.rows; row++) {
        for (int col = 0; col < img.cols; col++) {
            // Get pixel
            Vec3b pixel = img.at<Vec3b>(Point(row, col));
            // For each channel (B, G, R)
            for (int rgb = 0; rgb < 3; rgb++) {

                if (charPositionInText >= text.length()) {
                    // Save the encoded image
                    imwrite(imageFilepath, img);
                    return;
                }

                // Clear LSB Bit
                pixel.val[rgb] &= 0xFE;
                // Set LSB Bit
                pixel.val[rgb] |= (text[charPositionInText] >> bitPositionInChar) & 1;
                // Update bit and character position
                bitPositionInChar = (bitPositionInChar + 1) % 8;
                if (bitPositionInChar == 0) {
                    charPositionInText++;
                }
            }
            // Update Pixel
            img.at<Vec3b>(Point(row, col)) = pixel;
        }
    }
}

std::string Security::getKey() {

    Mat img = imread(imageFilepath);
    if (img.empty()) {
        std::cerr << "Error: Could not open or find the image!" << std::endl;
        return "";
    }

    std::string decodedText;
    char ch = 0;
    int bitPositionInChar = 0;

    for (int row = 0; row < img.rows; row++) {
        for (int col = 0; col < img.cols; col++) {
            Vec3b pixel = img.at<Vec3b>(Point(row, col));
            // For each channel (B, G, R)
            for (int rgb = 0; rgb < 3; rgb++) {

                if (decodedText.length() >= 93) {
                    // Message decoded completely
                    try {
                        json::parse(decodedText);
                        return decodedText;
                    }
                    catch (const nlohmann::json::exception& e) {
                        return "";
                    }
                }

                // Read Pixel
                Vec3b pixel = img.at<Vec3b>(Point(row, col));

                ch |= (pixel.val[rgb] & 0x01) << bitPositionInChar;

                bitPositionInChar = (bitPositionInChar + 1) % 8;
                if (bitPositionInChar == 0) {
                    decodedText += ch;
                    ch = 0;
                }
            }
        }
    }
    return "";
}
