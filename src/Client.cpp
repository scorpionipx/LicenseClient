#include "pch.h"

#include "Client.h"

// Constructors
Client::Client(
	const std::string applicationName, 
	const std::string applicationVersion, 
	const std::string publicKey
):
	validator(applicationName, applicationVersion) {
	// Store Vars
	this->applicationName = applicationName;
	this->applicationVersion = applicationVersion;
	this->publicKey = publicKey;
	this->security.setOwnerRSAPublicKey(publicKey);
}

// Functions
std::string Client::getMode() {
	return mode;
}

void Client::setMode(
	const std::string& mode,
	const std::string& licFilepath,
	const std::string& lmServer,
	const std::string& lmPort
) {
	if (mode != modeStandalone and mode != modeLicenseManager and mode != modeOnline) {
		return;
	}
	this->licFilepath = licFilepath;
	this->lmPort = lmPort;
	this->lmServer = lmServer;
	this->mode = mode;
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
	data["SerialNo"] = "WEUFMAIGWRG1245135";

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
		std::string licenseContent = server.request(server.requestTypeValidate, payload);
		result = licFile.loadContent(licenseContent);
	} else if (mode == modeStandalone) {
		result = licFile.loadFile(licFilepath);
	} else if (mode == modeLicenseManager) {
		result = false;
	}
	else {
		result = false;
	}
	return result;
}