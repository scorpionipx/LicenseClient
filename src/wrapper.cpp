#include "pch.h"
#include <iostream>

#include "Client.h"

extern "C"
{

	// License Client Initialization
	__declspec(dllexport) Client* LicenseClient(
		const char* applicationName, 
		const char* applicationVersion, 
		const char* publicKey,
		const char* keyStorage
	)	{
		try {
			return new Client(applicationName, applicationVersion, publicKey, keyStorage);
		} catch (const std::exception& ex) {
			std::cerr << "Error in Client initialization: " << ex.what() << std::endl;
			return nullptr;
		}
	}
	// LicenseClient Cleanup: Destructor
	__declspec(dllexport) void cleanup(Client* client) {
		delete client;
	}

	// Export Client Version
	__declspec(dllexport) const char* getVersion(Client* client) {
		const std::string& version = client->getVersion();
		return version.c_str();
	}

	// Export License Mode
	__declspec(dllexport) const char* getMode(Client* client) {
		const std::string& mode = client->getMode();
		return mode.c_str();
	}
	// Export Set License Mode
	__declspec(dllexport) void setMode(Client* client, const char* mode, const char* params[]) {
		std::string licFilepath = params[0];
		std::string serverUrl = params[1];
		std::string port = params[2];
		std::string serialNumber = params[3];

		client->setMode(mode, licFilepath, serverUrl, port, serialNumber);
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
			return false;
		}

	}

	// Export requestLicense function
	__declspec(dllexport) const char* requestLicense(Client* client) {
		const std::string& licenseData = client->collectData();
		return licenseData.c_str();
	}

	// Export LicFile
	__declspec(dllexport) const char* readLicense(const char* licenseFilepath, const char* keyStorage) {
		try {
			json content;
			Security security = Security(keyStorage);
			LicFile licFile = LicFile(&security);
			licFile.loadFile(licenseFilepath);
			
			content["application"] = licFile.getApplication();
			content["version"] = licFile.getVersion();
			content["serial_no"] = licFile.getSerialNo();
			content["expiry_date"] = licFile.getExpiryDate();
			return content.dump().c_str();
		} catch (const std::exception& ex) {
			std::cerr << "Error in readLicense: " << ex.what() << std::endl;
		}
		return nullptr;
	}
};