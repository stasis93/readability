#ifndef LIBCURL_H
#define LIBCURL_H

#include <vector>
#include <string>

namespace Curl
{
std::string fetchHtmlPage(const std::string& url);
}

#endif // LIBCURL_H
