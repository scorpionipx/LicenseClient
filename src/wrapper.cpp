#include "pch.h"
#include <iostream>

#include "Client.h"

extern "C"
{

	// License Client Initialization
	__declspec(dllexport) Client* LicenseClient(const char* applicationName, const char* applicationVersion, const char* publicKey)	{
		try {
			return new Client(applicationName, applicationVersion, publicKey);
		} catch (const std::exception& ex) {
			std::cerr << "Error in Client initialization: " << ex.what() << std::endl;
			throw std::invalid_argument("At least one argument is not valid.");
		}
	}
	// Export License Mode
	__declspec(dllexport) const char* getMode(Client* client) {
		const std::string& mode = client->getMode();
		return mode.c_str();
	}
	// Export Set License Mode
	__declspec(dllexport) void setMode(Client* client, const char* mode, const char* params[]) {
		std::string licFilepath = params[0];
		std::string lmServer = params[1];
		std::string lmPort = params[2];

		client->setMode(mode, licFilepath, lmServer, lmPort);
	}

	// Export SerialNo
	__declspec(dllexport) const char* getSerialNo(Client* client) {
		const std::string& serialNoStr = client->getSerialNo();
		return serialNoStr.c_str();
	}

	// Export Expire Date
	__declspec(dllexport) const char* getExpiryDate(Client* client) {
		const std::string& expireStr = client->getExpiryDate();
		return expireStr.c_str();
	}

	// Export isValid function
	__declspec(dllexport) bool isValid(Client* client) {
		return client->isValid();
	}

	// Export loadLicense function
	__declspec(dllexport) bool loadLicense(Client* client) {
		try {
			return client->loadLicense();
		} catch (const std::exception& ex) {
			std::cerr << "Error in loadLicense: " << ex.what() << std::endl;
		}

	}

	// Export requestLicense function
	__declspec(dllexport) const char* requestLicense(Client* client) {
		const std::string& licenseData = client->collectData();
		return licenseData.c_str();
	}

	// Export registerLicense
};