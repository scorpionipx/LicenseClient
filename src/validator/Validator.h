#pragma once

#include <string>

#include <nlohmann/json.hpp>

#include "../utils/utils.h"

using json = nlohmann::json;

class Validator {

private:
	json licenseContent;
public:
	// Constructor
	Validator(const std::string& applicationName, const std::string& applicationVersion);
	// Functions
	bool isValidApp(const std::string& applicationName);
	bool isValidVersion(const std::string& applicationVersion);
	bool isValidExpireDate(const std::string& expireDate);
	bool isValidConstraints();
};

