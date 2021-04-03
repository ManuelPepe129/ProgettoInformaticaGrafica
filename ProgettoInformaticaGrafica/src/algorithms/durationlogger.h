#pragma once

#include <string>
#include <iostream>
#include <time.h>

class DurationLogger
{
public:
	DurationLogger(const std::string& name);

	~DurationLogger();
private:
	std::string name;
	clock_t duration;
};

