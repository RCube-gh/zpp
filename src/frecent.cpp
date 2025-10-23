#include "frecent.hpp"
#include <ctime>

double frecent(double rank, std::time_t last_access) {
    std::time_t now = std::time(nullptr);
    double dx = static_cast<double>(now - last_access);
    return 10000.0 * rank * (3.75 / ((0.0001 * dx + 1.0) + 0.25));
}

