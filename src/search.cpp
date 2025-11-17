#include "search.hpp"
#include "frecent.hpp"
#include <algorithm>
#include <cctype>
#include <cstdio>
#include <sstream>
#include <filesystem>

using namespace std;
namespace fs = std::filesystem;

static string toLower(string s) {
    transform(s.begin(), s.end(), s.begin(),
              [](unsigned char c){ return tolower(c); });
    return s;
}

static vector<string> splitWords(const string& input) {
    istringstream iss(input);
    vector<string> words;
    string word;
    while (iss >> word) {
        words.push_back(word);
    }
    return words;
}


Searcher::Searcher(const vector<Entry>& entries) : entries(entries) {}

string Searcher::bestMatch(const string& query) const {
    string cwd = fs::current_path().string();
    vector<string> keywords = splitWords(query);
    if (keywords.empty()) return "";

    string last_keyword = toLower(keywords.back());
    vector<fs::path> local_candidates;

    // --- Search local directories first ---
    for (auto& p : fs::directory_iterator(cwd)) {
        if (!p.is_directory()) continue;
        string path_lower = toLower(p.path().string());
		string dir_name = toLower(p.path().filename().string());
		if (dir_name.find(last_keyword) == string::npos)continue;

        // AND search: all keywords must be found
        bool all_match = all_of(keywords.begin(), keywords.end(),
            [&](const string& kw){ return path_lower.find(toLower(kw)) != string::npos; });
        if (all_match)
            local_candidates.push_back(p.path());
    }

    if (!local_candidates.empty()) {
        // Sort local matches: prioritize directories whose names contain the last keyword
        sort(local_candidates.begin(), local_candidates.end(),
            [&](const fs::path& a, const fs::path& b) {
                string fa = toLower(a.filename().string());
                string fb = toLower(b.filename().string());
                bool a_suffix = fa.find(last_keyword) != string::npos;
                bool b_suffix = fb.find(last_keyword) != string::npos;
                return a_suffix > b_suffix;
            });
        return local_candidates.front().string();
    }

    // --- Search database entries ---
    vector<Entry> matches;
    for (auto& e : entries) {
		string dir_name = toLower(fs::path(e.path).filename().string());
		if (dir_name.find(last_keyword) == string::npos) continue;
        string path_lower = toLower(e.path);
        // AND search: all keywords must be contained in the path
        bool all_match = all_of(keywords.begin(), keywords.end(),
            [&](const string& kw){ return path_lower.find(toLower(kw)) != string::npos; });
        if (all_match)
            matches.push_back(e);
    }

    if (matches.empty()) return "";

    // Sort by suffix match and frecent score
    sort(matches.begin(), matches.end(), [&](const Entry& a, const Entry& b){
        string fa = toLower(fs::path(a.path).filename().string());
        string fb = toLower(fs::path(b.path).filename().string());
        bool a_suffix = fa.find(last_keyword) != string::npos;
        bool b_suffix = fb.find(last_keyword) != string::npos;

        if (a_suffix != b_suffix)
            return a_suffix; // suffix match first
        return frecent(a.rank, a.last_access) > frecent(b.rank, b.last_access);
    });

    return matches.front().path;
}


// getCompletions is currently not used

// vector<string> Searcher::getCompletions(const string& query, int limit) const {
//     string lower_query = toLower(query);
//     string cwd = fs::current_path().string();
//     vector<string> results;
//
//     // First, check subdirectories of current directory
//     for (auto& p : fs::directory_iterator(cwd)) {
//         if (p.is_directory()) {
//             string name = p.path().filename().string();
//             if (toLower(name).find(lower_query) != string::npos) {
//                 results.push_back(p.path().string());
//                 if (results.size() >= static_cast<size_t>(limit)) return results;
//             }
//         }
//     }
//
//     // Then, search database entries
//     vector<Entry> matches;
//     for (auto& e : entries) {
//         if (toLower(e.path).find(lower_query) != string::npos)
//             matches.push_back(e);
//     }
//
// 	sort(matches.begin(), matches.end(),
// 		 [&query](const Entry& a, const Entry& b) {
// 			 auto score = [&](const Entry& e) {
// 				 double base = frecent(e.rank, e.last_access);
// 				 std::string q = query;
// 				 std::string path = e.path;
//
// 				 // extract last dir name
// 				 std::string last = fs::path(path).filename().string();
//
// 				 double bonus = 0.0;
// 				 if (toLower(last) == toLower(q)) {
// 						bonus = 10000.0;  //precise match
// 				 } else if (toLower(path).find("/" + toLower(q)) != std::string::npos) {
// 						bonus = 100.0;  // directory match
// 				 }
//
// 				 return base + bonus;
// 			 };
// 			 return score(a) > score(b);
//      });
//
//     for (auto& m : matches) {
//         if (results.size() >= static_cast<size_t>(limit)) break;
//         results.push_back(m.path);
//     }
//
//     return results;
// }

