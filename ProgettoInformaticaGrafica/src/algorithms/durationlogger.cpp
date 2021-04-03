#include "durationlogger.h"


DurationLogger::DurationLogger(const std::string& name) : name(name)
{
    std::cout << ">>>>>>>>>>>>>> starting [ " << name << " ]\n";
    duration = clock();
}

DurationLogger::~DurationLogger()
{
    duration = clock() - duration;
    std::cout << "<<<<<<<<<<<<<< ending [ " << name << " ]: [ " << ((double)duration) / CLOCKS_PER_SEC << " ] sec\n";
}
