#pragma once

#ifdef LICLIENT_EXPORTS
#define LICLIENT_API __declspec(dllexport)
#else
#define LICLIENT_API __declspec(dllimport)
#endif

#include <iostream>
#include <string>

#include <nlohmann/json.hpp>
#include "server/Server.h"
#include "security/Security.h"
#include "licfile/LicFile.h"
#include "validator/Validator.h"

using json = nlohmann::json;

class Client {
private:
	std::string publicKey;
	// Mode: Standalone, Online, LicenseManager (Intranet)
	std::string mode;
	const std::string modeLicenseManager = "LicenseManager";
	const std::string modeOnline = "Online";
	const std::string modeStandalone = "Standalone";

	// Security
	Security security;
	// LicenseFile
	LicFile licFile = LicFile(&security);
	// Server
	Server server = Server(&security);
	// Validator
	Validator validator;

	std::string licFilepath;
	std::string lmServer;
	std::string lmPort;
public:
	// Constructor
	Client(
		const std::string applicationName,
		const std::string applicationVersion,
		const std::string publicKey
	);

	// Vars
	std::string applicationName;
	std::string applicationVersion;

	// Functions
	std::string collectData();
	bool loadLicense();

	std::string getMode();
	void setMode(
		const std::string& mode,
		const std::string& licFilepath,
		const std::string& lmServer,
		const std::string& lmPort
	);

	std::string getSerialNo();
	std::string getExpiryDate();

	bool isValid();
};

