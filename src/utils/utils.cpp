#include "pch.h"
#include <algorithm>
#include <chrono>
#include <ctime>
#include <iomanip>
#include <iostream>
#include <string>
#include <sstream>

// Convert a string to lowercase
std::string toLowerCase(const std::string& str) {
    std::string result = str;
    std::transform(result.begin(), result.end(), result.begin(), ::tolower);
    return result;
}

// Check Extension
bool hasExtension(const std::string& filename, const std::string& targetExtension) {
    size_t dotIndex = filename.find_last_of('.');

    if (dotIndex != std::string::npos) {
        std::string fileExtension = filename.substr(dotIndex + 1);
        return toLowerCase(fileExtension).compare(toLowerCase(targetExtension)) == 0;
    }
    return false;
}

std::string hexToString(const std::string& hex) {
    std::string result;
    for (size_t i = 0; i < hex.length(); i += 2) {
        // Extract two characters from the hexadecimal string
        std::string byteString = hex.substr(i, 2);

        // Convert the two characters to an integer
        char byte = static_cast<char>(std::stoi(byteString, nullptr, 16));

        // Append the character to the result
        result += byte;
    }
    return result;
}

bool validate_date(const std::string& expireDate)
{
    // Get the current date
    auto currentTime = std::chrono::system_clock::now();
    std::time_t currentTime_t = std::chrono::system_clock::to_time_t(currentTime);

    // Convert the current date to a string
    std::tm currentDate;
    localtime_s(&currentDate, &currentTime_t);
    std::stringstream currentDateString;
    currentDateString << std::put_time(&currentDate, "%d.%m.%Y");
    std::string currentDateStr = currentDateString.str();

    // Convert the string date to a std::tm structure
    std::tm dateFromStr = {};
    std::istringstream dateStream(expireDate);
    dateStream >> std::get_time(&dateFromStr, "%d.%m.%Y");

    // Compare the two dates
    if (std::difftime(std::mktime(&dateFromStr), currentTime_t) > 0) {
        // Valid
        return true;
    }
    else {
        // Expired
        return false;
    }
}

// Function to convert a string to its hexadecimal representation
std::string stringToHex(const std::string& input) {
    std::stringstream ss;
    ss << std::hex << std::setfill('0');

    for (char ch : input) {
        ss << std::setw(2) << static_cast<int>(static_cast<unsigned char>(ch));
    }

    return ss.str();
}