#include "pch.h"

#include "LicManager.h"

LicManager::LicManager(const std::string& applicationName, const std::string& applicationVersion, Security* security)
    : security(security),
    io_context(),
    executor(io_context.get_executor()),
    resolver(io_context),
    socket(io_context) {

    application = applicationName;
    version = applicationVersion;
    this->security = security;
}

LicManager::~LicManager() {
    release();
}

void LicManager::connect(std::string host, std::string port) {
    auto endpoints = resolver.resolve(host, port);
    // Connect to the server
    boost::asio::connect(socket, endpoints);

}

void LicManager::send(const std::string& message) {
    // Send data to the server
    boost::asio::write(socket, boost::asio::buffer(message));
}

std::string LicManager::receive() {
    // Receive data from the server
    char buffer[1024];
    size_t bytesReceived = socket.read_some(boost::asio::buffer(buffer, sizeof(buffer)));

    return std::string(buffer, bytesReceived);
}

void LicManager::exchange() {
    json payload;

    // Domain parameters
    OID CURVE = ASN1::secp256k1();
    AutoSeededX917RNG<AES> rng;
    ECDH < ECP >::Domain ecdh(CURVE);

    // Generate Keys
    SecByteBlock privKey(ecdh.PrivateKeyLength());
    SecByteBlock pubKey(ecdh.PublicKeyLength());
    ecdh.GenerateKeyPair(rng, privKey, pubKey);

    // Get X,Y Parameters
    DL_GroupParameters_EC<ECP> params(CURVE);
    ECPPoint p = params.DecodeElement(pubKey.BytePtr(), true);

    // Send Exchange Key Command
    payload["command"] = commandExchange;
    payload["payload"] = { {"x", IntToString(p.x)}, {"y", IntToString(p.y)} };
    send(payload.dump());

    // Receive public Key
    std::string response = receive();
    SecByteBlock publicKeyManager(response.size());
    publicKeyManager.Assign(reinterpret_cast<const byte*>(response.data()), response.size());

    // Compute shared key: kek
    SecByteBlock sharedKeyClient(ecdh.AgreedValueLength());

    // Agree with shared secret
    if (!ecdh.Agree(sharedKeyClient, privKey, publicKeyManager))
        throw std::runtime_error("Secret Client: NOK");

    std::string sharedKeyClientStr = stringToHex(std::string(reinterpret_cast<const char*>(sharedKeyClient.data()), sharedKeyClient.size()));

    // Send My Key
    byte initVector[AES::BLOCKSIZE];
    std::string ciphertext;
    std::string plainText = security->getAESKey();

    // Generate a random IV
    rng.GenerateBlock(initVector, sizeof(initVector));
    send(stringToHex(std::string(reinterpret_cast<const char*>(initVector), sizeof(initVector))));

    // Encrypt Key
    CBC_Mode<AES>::Encryption encryption(sharedKeyClient, sharedKeyClient.size(), initVector);
    StringSource(
        plainText,
        true,
        new StreamTransformationFilter(encryption, new StringSink(ciphertext))
    );
    send(stringToHex(ciphertext));
}

std::string LicManager::acquire() {
    json payload;

    // Key Exchange
    exchange();
    
    // Set command to "acquire"
    payload["command"] = commandAcquire;
    payload["payload"] = { { "application", application}, {"version" , version} };

    // Encrypt
    const std::string& cipherText = security->AESEncrypt(payload.dump());

    // Send
    send(cipherText);

    // Receive
    const std::string& response = receive();
    
    // Decrypt
    const std::string& plainText = security->AESDecrypt(response);

    // Parse
    json responseContent = json::parse(plainText);

    if (responseContent["return_code"] == 0) {
        acquiredSN = responseContent["serial_number"];
        return responseContent["license"];
    }

    return nullptr;
}

void LicManager::release() {
    json data;

    // Set command to "release"
    data["command"] = commandRelease;
    data["payload"] = { { "application", application}, {"version" , version}, {"serial_number", acquiredSN} };
    // Encrypt
    
    // Send
    std::string payload = data.dump();
    send(payload);

    // Receive
    json response = json::parse(receive());

    // Decrypt


    if (response["return_code"] == 0) {
        acquiredSN = "";
    }
}