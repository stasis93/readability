#include "textformatter.h"

#include <iterator>
#include <algorithm>
#include <sstream>
//#include <iostream>

#include "utils.h"

//using std::cout;
using std::endl;

namespace TextFormatter
{

string format(const vector<Myhtml::TextElement>& elems)
{
    using namespace Myhtml;
    std::ostringstream oss;

    for (const TextElement& el: elems)
    {
        if (el.tag == Tag::Text)
        {
            oss << Utils::removeNewLines(el.mainText);
        }
        else if (el.tag == Tag::H && !el.mainText.empty())
        {
            oss << endl
                << endl
                << "## " << Utils::removeNewLines(el.mainText) << " ##";
        }
        else if (el.tag == Tag::P || el.tag == Tag::Br)
        {
            oss << endl << endl;
        }
        else if (el.tag == Tag::Img && !el.mainText.empty())
        {
            oss << " {" << Utils::removeNewLines(el.mainText);
            if (!el.extraText.empty())
                oss << ", " << Utils::removeNewLines(el.extraText);
            oss << "} ";
        }
        else if (el.tag == Tag::A && !el.mainText.empty())
        {
            oss << " [" << Utils::removeNewLines(el.mainText) << "] ";
            if (!el.extraText.empty())
                oss << " " << Utils::removeNewLines(el.extraText);
        }
    }
    return oss.str();
}

string splitWithWidth(const string& text, int maxWidth)
{
    std::ostringstream oss;

    for (const string& line: Utils::split(text, '\n'))
    {
        int currentWidth = 0;
        for (const string& word: Utils::split(line, ' '))
        {
            int wordLen = Utils::utf8Len(word.c_str());

            if (currentWidth + wordLen + 1 < maxWidth)
            {
                oss << word << " ";
                currentWidth += (wordLen + 1);
            }
            else if (wordLen > 40) // probably a long link, let's split it
            {
                string word_(word);
                while (word_.size() > maxWidth - currentWidth - 1)
                {
                    string tail = word_.substr(maxWidth - currentWidth - 1);
                    word_ = word_.substr(0, maxWidth - currentWidth - 1);

                    oss << word_ << endl;
                    currentWidth = 0;

                    word_ = tail;
                }
                oss << word_;
                currentWidth = wordLen;
            }
            else
            {
                oss << endl << word << " ";
                currentWidth = wordLen + 1;
            }
        }
        oss << endl;
    }
    return oss.str();
}

}
