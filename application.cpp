#include "application.h"
#include "libcurl.h"
#include "textformatter.h"
#include "utils.h"

#include <iostream>
#include <fstream>
#include <algorithm>


static const std::string UsageMsg = "Usage: program_name URL\nor program_name -f html_file";
static const std::string DefaultOutputFile = "output.txt";


void Application::run(const std::vector<std::string>& args)
{
    std::string html;
    std::string outputFileName = DefaultOutputFile;

    bool fromFile = false;

    if (args.size() == 2 && args[0] == "-f")
        fromFile = true;
    else if (args.size() != 1) {
        printMsg(UsageMsg);
        return;
    }

    if (fromFile) {
        printMsg("Reading from " + args[1]);
        html = Utils::readText(args[1]);
    }
    else {
        printMsg("Fetching html page...");
        html = Curl::fetchHtmlPage(args[0]);
    }

    if (!html.empty())
    {
        printMsg("Parsing...");

        Myhtml::Document doc;
        doc.parse(html);
        std::vector<Myhtml::TextElement> parseResult = doc.textElements();

        if (parseResult.empty())
            printMsg("Fail");
        else
        {
            printMsg("Formatting...");
            std::string result = TextFormatter::format(parseResult);
            result = TextFormatter::splitWithWidth(result);

            if (!fromFile)
                outputFileName = Utils::pathFromURL(args[0]);

            printMsg("Writing to " + outputFileName);
            Utils::writeText(result, outputFileName);
        }
    }
    else
    {
        printMsg("Can't load html page / open file");
    }

}

void Application::printMsg(const std::string& msg)
{
    std::cout << msg << std::endl;
}
