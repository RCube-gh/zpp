#include "search.hpp"
#include "frecent.hpp"
#include <algorithm>
#include <cctype>
#include <filesystem>

using namespace std;
namespace fs = std::filesystem;

static string toLower(string s) {
    transform(s.begin(), s.end(), s.begin(),
              [](unsigned char c){ return tolower(c); });
    return s;
}

Searcher::Searcher(const vector<Entry>& entries) : entries(entries) {}

string Searcher::bestMatch(const string& query) const {
    string lower_query = toLower(query);
    string cwd = fs::current_path().string();

    for (auto& p : fs::directory_iterator(cwd)) {
        if (p.is_directory()) {
            string name = p.path().filename().string();
            if (toLower(name).find(lower_query) != string::npos)
                return p.path().string();
        }
    }

    vector<Entry> matches;
    for (auto& e : entries) {
        if (toLower(e.path).find(lower_query) != string::npos)
            matches.push_back(e);
    }

    if (matches.empty()) return "";

    sort(matches.begin(), matches.end(),
         [](const Entry& a, const Entry& b) {
             return frecent(a.rank, a.last_access) >
                    frecent(b.rank, b.last_access);
         });

    return matches.front().path;
}

vector<string> Searcher::getCompletions(const string& query, int limit) const {
    string lower_query = toLower(query);
    string cwd = fs::current_path().string();
    vector<string> results;

    // First, check subdirectories of current directory
    for (auto& p : fs::directory_iterator(cwd)) {
        if (p.is_directory()) {
            string name = p.path().filename().string();
            if (toLower(name).find(lower_query) != string::npos) {
                results.push_back(p.path().string());
                if (results.size() >= static_cast<size_t>(limit)) return results;
            }
        }
    }

    // Then, search database entries
    vector<Entry> matches;
    for (auto& e : entries) {
        if (toLower(e.path).find(lower_query) != string::npos)
            matches.push_back(e);
    }

	sort(matches.begin(), matches.end(),
		 [&query](const Entry& a, const Entry& b) {
			 auto score = [&](const Entry& e) {
				 double base = frecent(e.rank, e.last_access);
				 std::string q = query;
				 std::string path = e.path;

				 // extract last dir name
				 std::string last = fs::path(path).filename().string();

				 double bonus = 0.0;
				 if (toLower(last) == toLower(q)) {
						bonus = 10000.0;  //precise match
				 } else if (toLower(path).find("/" + toLower(q)) != std::string::npos) {
						bonus = 100.0;  // directory match
				 }

				 return base + bonus;
			 };
			 return score(a) > score(b);
     });

    for (auto& m : matches) {
        if (results.size() >= static_cast<size_t>(limit)) break;
        results.push_back(m.path);
    }

    return results;
}

