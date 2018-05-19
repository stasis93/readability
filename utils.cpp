#include "utils.h"

#include <fstream>
#include <iostream>
#include <sstream>
#include <stdexcept>
#include <algorithm>

#include <boost/algorithm/string/replace.hpp>
#include <boost/algorithm/string/trim.hpp>
#include <boost/algorithm/string/split.hpp>
#include <boost/algorithm/string/predicate.hpp>

//#include <boost/filesystem/fstream.hpp>
#include <boost/filesystem/operations.hpp>

namespace ba = boost::algorithm;
namespace bf = boost::filesystem;

static const std::string FallbackPath = "output.txt";

namespace Utils
{

bool mkDirs(const std::string& path)
{
    std::vector<std::string> parts;
    ba::split(parts, path, [](char c){return c == '/';});
    bf::path p(".");

    try {
        for (const std::string& part: parts)
        {
            if (!bf::exists(p) || !bf::is_directory(p))
                bf::create_directory(p);

            p = p / part; // "/" -- append
        }
    }
    catch (bf::filesystem_error& ex) {
        std::cerr << ex.what() << std::endl;
        return false;
    }
    return true;
}

std::string readText(const std::string& path)
{
    std::string data;
    std::ifstream ifs(path);
    if (!ifs.is_open()) {
        throw std::runtime_error("Can't open file " + path);
    } else {
        std::stringstream ss;
        ss << ifs.rdbuf();
        data = ss.str();
        ifs.close();
    }
    return data;
}

void writeText(const std::string& text, const std::string& path)
{
//    std::ofstream ofs(mkDirs(path) ? path : FallbackPath);
    std::string path_(path);
    if (!mkDirs(path)) {
        path_ = FallbackPath;
        std::cerr << "Can't create directories, writing to " << FallbackPath << std::endl;
    }

    std::ofstream ofs(path_);

    if (!ofs.is_open()) {
        throw std::runtime_error("Can't open " + path);
    } else {
        ofs << text;
        ofs.close();
    }
}

std::string pathFromURL(std::string url)
{
    if (url.back() == '/')
        url = url.substr(0, url.size() - 1);

    auto pos = url.find("//");
    if (pos != std::string::npos)
        url = url.substr(pos + 2);

    auto it = std::remove_if(url.begin(), url.end(), [](char c) {
        return c == '\\' /*|| c == '/'*/
                || c == ':'  || c == '*'
                || c == '?'  || c == '"'
                || c == '<'  || c == '>'
                || c == '|';
    });
    url.erase(it, url.end());

    return url + ".txt";
}

int utf8Len(const char* p)
{
    /* Count bytes that don't match 10xxxxxx */
    int len = 0;
    while (*p) len += ((*p++ & 0xc0) != 0x80);
    return len;
}

std::vector<std::string> split(const std::string& text, char delim)
{
    std::vector<std::string> parts;
    ba::split(parts, text, [delim](char c){return c == delim;});
    return parts;
}

bool isEmpty(const std::string& str)
{
    return std::all_of(str.begin(), str.end(), [](char c){
        return c == '\n' || c == '\t' || c == ' ';
    });
}

std::string removeNewLines(std::string text)
{
    return
    ba::replace_all_copy(
        ba::replace_all_copy(
            ba::trim_copy_if(
                text, ba::is_any_of("\r\n")
            ),
            "\n", " "
        ),
        "\r", " "
    );
}

bool containsNewLine(const std::string& text)
{
    return text.find("\n") != std::string::npos
        || text.find("\r") != std::string::npos;
}

}
