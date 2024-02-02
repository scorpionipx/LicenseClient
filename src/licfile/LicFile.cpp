#include "pch.h"
#include "LicFile.h"

namespace fs = std::filesystem;

LicFile::LicFile(Security* security) {
	this->security = security;
}

LicFile::~LicFile() {
	// Destructor implementation
}

bool LicFile::loadFile(const std::string& filePath) {
	if (filePath == "") {
		return false;
	}

	// Check FilePath existence
	if (!fs::exists(filePath)) {
		std::cerr << "File not found at \"" + filePath + "\"." << std::endl;
		return false;
	}
	// Check FilePath extension
	if (hasExtension(filePath, "lic") == false) {
		std::cerr << "File extension is invalid." << std::endl;
		return false;
	}

	// Open a file for reading
	std::ifstream inputFile(filePath);

	// Check if the file is open
	if (!inputFile.is_open()) {
		std::cerr << "Error opening the file." << std::endl;
		return false;
	}

	// Read the contents of the file
	std::ostringstream buffer;
	buffer << inputFile.rdbuf();
	std::string fileContent = buffer.str();

	// Close the file
	inputFile.close();

	if (fileContent == "") {
		std::cerr << "LicFile is empty." << std::endl;
		return false;
	}

	if (loadContent(fileContent) == false) {
		return false;
	}
	this->filePath = filePath;
	return true;
}

bool LicFile::loadContent(const std::string& content) {
	this->filePath = "";

	// Decrypt Content
	this->fileContent = security->AESDecrypt(content);

	// Parse
	if (parse() == false) {
		this->fileContent = "";
		return false;
	};
	return true;
}

bool LicFile::parse() {
	// std::string content = hexToString(this->fileContent);
	try {
		licenseContent = json::parse(this->fileContent);
	}
	catch (const nlohmann::json::exception& e) {
		std::cerr << "Error parsing JSON: " << e.what() << std::endl;
		std::cerr << "LicFile is corrupted." << std::endl;
		return false;
	}
	return true;
}

std::string LicFile::getSerialNo() {
	if (licenseContent.contains("serial_no"))
		return licenseContent["serial_no"];
	return "";
}
std::string LicFile::getApplication() {
	if (licenseContent.contains("application"))
		return licenseContent["application"];
	return "";
}
std::string LicFile::getVersion() {
	if (licenseContent.contains("version"))
		return licenseContent["version"];
	return "";
}
std::string LicFile::getExpiryDate() {
	if (licenseContent.contains("expiry_date"))
		return licenseContent["expiry_date"];
	return "";
}