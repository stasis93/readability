#include "libcurl.h"

#include <vector>
#include <iostream>

#include <curl/curl.h>


namespace Curl
{

class Handle
{
public:
    ~Handle();
    static CURL* get();
    Handle(Handle&) = delete;
    Handle& operator=(Handle&) = delete;
private:
    Handle();
    CURL* m_handle;
};

Handle::Handle()
{
    m_handle = curl_easy_init();
}

Handle::~Handle()
{
    curl_easy_cleanup(m_handle);
}

CURL* Handle::get()
{
    // "lazy" initialization
    static Handle instance;
    return instance.m_handle;
}

// ============================================================================

size_t writeCallback(char* ptr, size_t size, size_t nmemb, void* userdata)
{
    size_t realSize = size * nmemb;
    auto& data = *reinterpret_cast<std::string*>(userdata);
    data.insert(data.end(), ptr, ptr + realSize);
    return realSize;
}

std::string fetchHtmlPage(const std::string& url)
{
    auto handle = Handle::get();
    std::string data;

    curl_easy_setopt(handle, CURLOPT_USERAGENT, "libcurl-agent/1.0");
    curl_easy_setopt(handle, CURLOPT_URL, url.c_str());
    curl_easy_setopt(handle, CURLOPT_WRITEFUNCTION, writeCallback);
    curl_easy_setopt(handle, CURLOPT_WRITEDATA, reinterpret_cast<void*>(&data));

    CURLcode result;
    if (CURLE_OK != (result = curl_easy_perform(handle)))
        std::cerr << "curl_easy_perform() failed! Error code: " << result << std::endl;

    return data;
}

}
