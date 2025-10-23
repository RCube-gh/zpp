#include "cli.hpp"
#include "frecent.hpp"
#include "search.hpp"
#include "database.hpp"
#include <iostream>
#include <filesystem>

int CLI::run(int argc, char* argv[]) {
    Database db;
    db.load();
    Searcher searcher(db.getEntries());

    // --init: print shell hook
    if (argc > 1 && std::string(argv[1]) == "--init") {
        std::cout << "PROMPT_COMMAND='(z++ --add \"$(pwd)\" & disown)'\n";
        return 0;
    }

    // --add: auto record current dir
    if (argc > 1 && std::string(argv[1]) == "--add") {
        if (argc < 3) return 0;
        std::string path = argv[2];
        if (std::filesystem::exists(path)) db.addOrUpdate(path);
        return 0;
    }

    // z++ query
    if (argc < 2) {
        std::cerr << "Usage: z++ [keyword]\n";
        return 1;
    }

    std::string keyword = argv[1];
    std::string dest = searcher.bestMatch(keyword);

    if (dest.empty()) {
        std::cerr << "No match found.\n";
        return 1;
    }

    std::cout << dest;
    return 0;
}

