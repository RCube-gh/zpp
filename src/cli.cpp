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
        std::cout << R"SH(
_zpp_hook() {
    (zpp --add "$(pwd)" >/dev/null 2>&1) &
	disown

}

if [[ "$PROMPT_COMMAND" != *"_zpp_hook"* ]]; then
    PROMPT_COMMAND="_zpp_hook; $PROMPT_COMMAND"
fi

alias z++='zpp'
)SH";
        return 0;
        }


    // --add: auto record current dir
    if (argc > 1 && std::string(argv[1]) == "--add") {
        if (argc < 3) return 0;
		std::filesystem::path abs_path=std::filesystem::absolute(argv[2]);
        if (std::filesystem::exists(abs_path)) db.addOrUpdate(abs_path.string());
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

