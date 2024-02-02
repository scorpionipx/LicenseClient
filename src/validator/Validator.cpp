#include "pch.h"
#include "Validator.h"


Validator::Validator(const std::string& applicationName, const std::string& applicationVersion) {
	if (applicationName == "") {
		std::cerr << "Application Name was not provided." << std::endl;
		return;
	}
	if (applicationVersion == "") {
		std::cerr << "Application Version was not provided." << std::endl;
		return;
	}
	// Store Data
	licenseContent["application"] = applicationName;
	licenseContent["version"] = applicationVersion;
	// TODO: Check constraints.
}

bool Validator::isValidApp(const std::string& applicationName) {
	bool result = bool(toLowerCase(applicationName).compare(toLowerCase(licenseContent["application"])));
	if (result) {
		std::cout << "Application Name is not valid." << std::endl;
	}
	return !result;
}

bool Validator::isValidVersion(const std::string& applicationVersion) {
	bool result = bool(toLowerCase(applicationVersion).compare(toLowerCase(licenseContent["version"])));
	if (result) {
		std::cout << "Application Version is not valid." << std::endl;
	}
	return !result;
}

bool Validator::isValidExpireDate(const std::string& expireDate) {
	bool result = validate_date(expireDate);
	if (!result) {
		std::cout << "License is expired." << std::endl;
	}
	return result;
}
