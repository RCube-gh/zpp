#pragma once
#include"database.hpp"
#include"frecent.hpp"
#include<string>
#include<vector>

class Searcher{
public:
	explicit Searcher(const std::vector<Entry>& entries);
	std::vector<Entry> search(const std::string& query)const;
	std::string bestMatch(const std::string& query)const;
	std::vector<std::string> getCompletions(const std::string& query, int limit = 10)const;

private:
	const std::vector<Entry>& entries;
};
