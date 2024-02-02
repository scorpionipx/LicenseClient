#include "pch.h"
#include "Server.h"

Server::Server(Security* security) {
    this->security = security;
}

static size_t responseWriteCallback(void* contents, size_t size, size_t nmemb, void* userp) {
     ((std::string*)userp)->append((char*)contents, size * nmemb);
     return size * nmemb;
}

std::string Server::get(const std::string& url) {
    std::string response;
    CURL* curl = curl_easy_init();

    try {
        curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
        curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, &responseWriteCallback);
        curl_easy_setopt(curl, CURLOPT_WRITEDATA, &response);

        CURLcode res = curl_easy_perform(curl);

        if (res != CURLE_OK) {
            std::cerr << "Failed to perform HTTP GET request: " << curl_easy_strerror(res) << std::endl;
        }

        curl_easy_cleanup(curl);
    } catch (const std::exception& e) {
        std::cerr << "Exception caught for HTTP GET request: " << e.what() << std::endl;
        return nullptr;
    }

    return response;
}

std::string Server::post(const std::string& url, const std::string& postData) {
    std::string readBuffer;

    try {
        CURL* curl = curl_easy_init();

        curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
        curl_easy_setopt(curl, CURLOPT_POST, 1L);
        curl_easy_setopt(curl, CURLOPT_POSTFIELDS, postData.c_str());
        curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, &responseWriteCallback);
        curl_easy_setopt(curl, CURLOPT_WRITEDATA, &readBuffer);

        CURLcode res = curl_easy_perform(curl);

        // Check for errors
        if (res != CURLE_OK) {
            throw std::runtime_error(curl_easy_strerror(res));
        }

        curl_easy_cleanup(curl);
    } catch (const std::exception& e) {
        std::cerr << "Exception caught for HTTP POST request: " << e.what() << std::endl;
        return nullptr;
    }

    return readBuffer;
}

std::string Server::request(const std::string& requestType, const std::string& payload) {
    std::ostringstream url;
    
    url << server << requestType;

    std::string response = post(url.str(), payload);

    return post(url.str(), payload);
}