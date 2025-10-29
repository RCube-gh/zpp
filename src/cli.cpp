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
	//
	if (argc > 1 && std::string(argv[1]) == "--init") {
		std::cout << R"SH(
# Initialize completion system if not already done
if [[ -z "$_comps" ]]; then
    autoload -Uz compinit
    compinit
fi

# zpp zsh hook
_zpp_hook() {
(set +m; zpp --add "$(pwd)" >/dev/null 2>&1 &)
}

# append hook if not already set
if [[ -z "$precmd_functions" ]] || [[ "${precmd_functions[(r)_zpp_hook]}" != "_zpp_hook" ]]; then
precmd_functions+=(_zpp_hook)
fi

# define wrapper function for jumping
z++() {
    local dest
    dest="$(command zpp "$@")"
    if [[ -n "$dest" && -d "$dest" ]]; then
        cd "$dest"
    else
        echo "No match found." >&2
        return 1
    fi
}

# zsh completion function for z++
_zpp_completion() {
    local -a completions
    local query="${words[2]}"
    
    # Get completions from zpp
    if [[ -n "$query" ]]; then
        completions=("${(@f)$(zpp --complete "$query" 2>/dev/null)}")
    else
        completions=("${(@f)$(zpp --complete "" 2>/dev/null)}")
    fi
    
    # Offer completions to zsh
    if [[ ${#completions[@]} -gt 0 ]]; then
        compadd -U -V directories -a completions
    fi
}

# Register completion
compdef _zpp_completion z++
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

    // --complete: return list of matching directories for tab completion
    if (argc > 1 && std::string(argv[1]) == "--complete") {
        std::string keyword = (argc > 2) ? argv[2] : "";
        auto completions = searcher.getCompletions(keyword);
        for (const auto& path : completions) {
            std::cout << path << "\n";
        }
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

