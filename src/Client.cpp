#include "pch.h"

#include "Client.h"

// Constructors
Client::Client(
	const std::string applicationName, 
	const std::string applicationVersion, 
	const std::string publicKey,
	const std::string keyStorage
):
	validator(applicationName, applicationVersion),
	security(keyStorage),
	licManager(applicationName, applicationVersion, &security)
{
	// Store Vars
	this->applicationName = applicationName;
	this->applicationVersion = applicationVersion;
	this->publicKey = publicKey;
	this->security.setOwnerRSAPublicKey(publicKey);
}

std::string Client::getVersion() {
	return clientVersion;
}

// Functions
std::string Client::getMode() {
	return mode;
}

void Client::setMode(
	const std::string& mode,
	const std::string& licFilepath,
	const std::string& serverUrl,
	const std::string& port,
	const std::string& serialNumber
) {
	if (mode != modeStandalone and mode != modeLicenseManager and mode != modeOnline) {
		return;
	}
	this->licFilepath = licFilepath;
	this->port = port;
	this->serverUrl = serverUrl;
	this->mode = mode;
	this->serialNo = serialNumber;
}

std::string Client::getSerialNo() {
	return licFile.getSerialNo();
}

std::string Client::getExpiryDate() {
	return licFile.getExpiryDate();
}


bool Client::isValid() {
	bool isValid = true;
	if (mode == "standalone") {
		// Validate Application Name
		isValid = this->validator.isValidApp(this->licFile.getApplication());
		if (isValid == false) return isValid;
		// Validate Application Version
		isValid = this->validator.isValidVersion(this->licFile.getVersion());
		if (isValid == false) return isValid;
		// Validate Expire Date
		isValid = this->validator.isValidExpireDate(this->licFile.getExpiryDate());
		if (isValid == false) return isValid;
	}
	return isValid;
}

std::string Client::collectData() {
	json data;

	data = json::parse(security.getAESKey());
	data["SerialNo"] = serialNo;

	// Encrypt Data
	std::string cipherPayload = stringToHex(security.RSAEncrypt(data.dump()));

	return cipherPayload;
}

bool Client::loadLicense() {
	
	bool result = false;

	if (mode == modeOnline) {
		// Collect Data & Key Exchange
		const std::string payload = collectData();
		// Authentificate for validation
		server.setUrl(serverUrl);
		std::string licenseContent = server.request(server.requestTypeValidate, payload);
		result = licFile.loadContent(licenseContent);
	} else if (mode == modeStandalone) {
		result = licFile.loadFile(licFilepath);
	} else if (mode == modeLicenseManager) {
		licManager.connect(serverUrl, port);
		// Collect Data & Key Exchange
		std::string licenseContent = licManager.acquire();
		result = licFile.loadContent(licenseContent);
	}
	else {
		result = false;
	}
	return result;
}