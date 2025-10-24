#include "database.hpp"
#include <fstream>
#include <sstream>
#include <ctime>
#include <filesystem>
#include <unistd.h>

namespace fs = std::filesystem;


static fs::path getExecutableDir() {
    char buf[1024];
    ssize_t len = readlink("/proc/self/exe", buf, sizeof(buf) - 1);
    if (len != -1) {
        buf[len] = '\0';
        return fs::path(buf).parent_path();
    }
    // fallback
    return fs::current_path();
}

Database::Database(const std::string& filepath) {
    if (!filepath.empty())
        dbPath = filepath;
    else {
        //const char* home = std::getenv("HOME");
        //dbPath = std::string(home ? home : ".") + "/.zpp.db";

        fs::path exeDir = getExecutableDir();
        fs::path dataDir = exeDir / "data";

        if (!fs::exists(dataDir))
            fs::create_directory(dataDir);

        dbPath = (dataDir / "zpp.db").string();
    }
}

void Database::load() {
    entries.clear();
    std::ifstream file(dbPath);
    if (!file) return;

    std::string line;
    while (std::getline(file, line)) {
        std::stringstream ss(line);
        std::string dir;
        double rank;
        std::time_t last;
        if (std::getline(ss, dir, '|') && ss >> rank && ss.ignore() && ss >> last) {
            if (fs::exists(dir))
                entries.push_back({dir, rank, last});
        }
    }
}

void Database::save() const {
    std::ofstream file(dbPath, std::ios::trunc);
    for (auto& e : entries)
        file << e.path << "|" << e.rank << "|" << e.last_access << "\n";
}

Entry* Database::findEntry(const std::string& path) {
    for (auto& e : entries)
        if (e.path == path)
            return &e;
    return nullptr;
}

void Database::addOrUpdate(const std::string& path) {
    std::time_t now = std::time(nullptr);
    if (Entry* e = findEntry(path)) {
        e->rank += 1;
        e->last_access = now;
    } else {
        entries.push_back({path, 1.0, now});
    }
    save();
}

