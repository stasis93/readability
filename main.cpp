#include <iostream>
#include <stdexcept>

#include "application.h"

int main(int argc, char* argv[])
{
    std::vector<std::string> args;
    for (auto i = 1; i < argc; i++)
        args.emplace_back(argv[i]);

    try
    {
        Application().run(args);
    }
    catch (std::exception& ex)
    {
        std::cerr << "EXCEPTION | " << ex.what() << std::endl;
    }
    catch (...)
    {
        std::cerr << "UNKNOWN EXCEPTION" << std::endl;
    }
    return 0;
}
