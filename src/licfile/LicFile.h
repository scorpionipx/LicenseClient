#pragma once

#ifdef LICFILE_EXPORTS
#define LICFILE_API __declspec(dllexport)
#else
#define LICFILE_API __declspec(dllimport)
#endif

#include <stdexcept>
#include <string>
#include <filesystem>
#include <fstream>
#include <iostream>

#include <nlohmann/json.hpp>

#include "../utils/utils.h"
#include "../security/Security.h"

using json = nlohmann::json;

class LicFile {
public:
    // Constructor & Destructor
    LicFile(Security* security);
    ~LicFile();
    // Functions
    bool loadContent(const std::string& content);
    bool loadFile(const std::string& filePath);

    std::string getSerialNo();
    std::string getApplication();
    std::string getVersion();
    std::string getExpiryDate();
private:
    Security* security;
    // Vars
    std::string filePath;
    std::string fileContent;
    json licenseContent;
    // Functions
    bool parse();
};