#ifndef UTILS_H_INCLUDED
#define UTILS_H_INCLUDED

#include <string>
#include <vector>


namespace Utils
{

std::string readText(const std::string& fileName);

void writeText(const std::string& text, const std::string& fileName);

std::string pathFromURL(std::string url);

int utf8Len(const char* p);

std::vector<std::string> split(const std::string& text, char delim = ' ');

bool isEmpty(const std::string& str);

std::string removeNewLines(std::string text);

bool containsNewLine(const std::string& text);

}

#endif // UTILS_H_INCLUDED
