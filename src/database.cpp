#include "database.hpp"
#include <fstream>
#include <sstream>
#include <ctime>
#include <filesystem>

void Database::load() {
    entries.clear();
    std::ifstream file(path);
    if (!file) return;

    std::string line;
    while (std::getline(file, line)) {
        std::stringstream ss(line);
        std::string dir;
        double rank;
        std::time_t last;
        if (std::getline(ss, dir, '|') && ss >> rank && ss.ignore() && ss >> last) {
            Entry e{dir, rank, last};
            if (std::filesystem::exists(e.path))
                entries.push_back(e);
        }
    }
}

void Database::save() const {
    std::ofstream file(path, std::ios::trunc);
    for (auto& e : entries)
        file << e.path << "|" << e.rank << "|" << e.last_access << "\n";
}

void Database::addOrUpdate(const std::string& dir) {
    std::time_t now = std::time(nullptr);
    for (auto& e : entries) {
        if (e.path == dir) {
            e.rank += 1;
            e.last_access = now;
            save();
            return;
        }
    }
    entries.push_back({dir, 1.0, now});
    save();
}

const std::vector<Entry>& Database::getEntries() const {
    return entries;
}

