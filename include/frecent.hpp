#pragma once
#include <ctime>

// Calculate "frecency" score based on rank and last access time
double frecent(double rank, std::time_t last_access);
