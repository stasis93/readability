#ifndef TEXTFORMATTER_H
#define TEXTFORMATTER_H

#include <vector>
#include <string>

#include "myhtml.h"

using std::vector;
using std::string;

namespace TextFormatter
{


string splitWithWidth(const std::string& text, int maxWidth = 80);
string format(const vector<Myhtml::TextElement>& elems);

}

#endif // TEXTFORMATTER_H
