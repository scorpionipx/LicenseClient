#pragma once

#include <iostream>
#include <string>
#include <sstream>

#include <curl/curl.h>
#include "../security/Security.h"


static size_t responseWriteCallback(void* contents, size_t size, size_t nmemb, void* userp);

class Server {
private:
	// Server for Online mode.
	// const std::string server = "http://ia00074vma:6996/";
	std::string server = "http://localhost:6969/";

	Security* security;

	// Functions
	std::string get(const std::string& url);
	std::string post(const std::string& url, const std::string& payload);
public:
	// Constructor
	Server(Security* security);

	const std::string requestTypeRegister = "licenses/register/";
	const std::string requestTypeValidate = "licenses/get/";

	// Functions
	bool heartbeat();
	void setUrl(const std::string& server);
	std::string request(const std::string& requestType, const std::string& payload);
};

