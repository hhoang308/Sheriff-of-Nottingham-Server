#include "Log.h"
#include <ctime>
#include <sstream>
#include <iomanip>

const std::string getLogFileName()
{
    auto t = std::time(nullptr);
    auto tm = *std::localtime(&t);

    std::ostringstream oss;
    oss << "log/log" << "_"
        << std::put_time(&tm, "%Y%m%d_%H%M%S") << ".txt";

    return oss.str();
}