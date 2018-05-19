#ifndef APPLICATION_H
#define APPLICATION_H

#include <vector>
#include <string>


class Application
{
public:
    void run(const std::vector<std::string>& args);

private:
    void printMsg(const std::string& msg);
};

#endif // APPLICATION_H
