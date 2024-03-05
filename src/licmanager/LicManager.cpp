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

std::string LicManager::exchange() {
    json payload;
    std::string response;

    std::string sharedKeyClientStr;
    std::string initVectorStr;

    std::string plainText;
    std::string cipherText;

    byte initVector[AES::BLOCKSIZE];

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
    payload = { {"x", IntToString(p.x)}, {"y", IntToString(p.y)} };
    send(payload.dump());

    // Receive public Key
    response = receive();
    SecByteBlock publicKeyManager(response.size());
    publicKeyManager.Assign(reinterpret_cast<const byte*>(response.data()), response.size());

    // Compute shared key: kek
    SecByteBlock sharedKeyClient(ecdh.AgreedValueLength());

    // Agree with shared secret
    if (!ecdh.Agree(sharedKeyClient, privKey, publicKeyManager))
        throw std::runtime_error("Secret Client: NOK");

    sharedKeyClientStr = std::string(reinterpret_cast<const char*>(sharedKeyClient.data()), sharedKeyClient.size());

    // Send IV && Generate a random IV
    rng.GenerateBlock(initVector, sizeof(initVector));
    initVectorStr = std::string(reinterpret_cast<const char*>(initVector), sizeof(initVector));
    send(stringToHex(initVectorStr));

    // Get and Decrypt Manager Keys
    response = receive();

    // Send and Encrypt Client Keys
    cipherText = security->AESEncrypt(security->getAESKey(), sharedKeyClientStr, initVectorStr);
    send(stringToHex(cipherText));

    return sharedKeyClientStr;
}

std::string LicManager::acquire() {
    json payload;

    // Initiate Acquire Process
    payload["command"] = commandAcquire;
    payload["content"] = { { "application", application}, {"version" , version} };
    send(payload.dump());

    // Wait to see if there is any License available
    json response = json::parse(receive());
    if (response["return_code"] == 1)
        return nullptr;

    // Key Exchange
    const std::string& sharedKey = exchange();

    // Encrypt with Client Key
    // const std::string& cipherText = security->AESEncrypt(payload.dump());
    // Send
    // send(cipherText);

    // Receive License
    response = receive();
    // Decrypt with Manager Key
    const std::string& plainTextManager = security->AESDecrypt(response, sharedKey);

    return plainTextManager;
}

void LicManager::release() {
    json payload;

    // Set command to "release"
    payload["command"] = commandRelease;
    payload["payload"] = { { "application", application}, {"version" , version}, {"serial_number", acquiredSN} };
    
    // Encrypt with Client Key
    const std::string& cipherText = security->AESEncrypt(payload.dump());
    
    // Send
    send(cipherText);

    acquiredSN = "";
}