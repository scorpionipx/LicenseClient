#pragma once

#include <iostream>

// Convert a string to lowercase
std::string toLowerCase(const std::string& str);

// Check Extension
bool hasExtension(const std::string& filename, const std::string& targetExtension);

std::string hexToString(const std::string& hex);

bool validate_date(const std::string& expireDate);

std::string stringToHex(const std::string& input);