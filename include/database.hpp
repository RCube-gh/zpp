#pragma once
#include<string>
#include<vector>
#include<ctime>

struct Entry{
	std::string path;
	double rank;
	std::time_t last_access;
};


class Database{
public:
	explicit Database(const std::string& filepath="");
	void load();
	void save() const;
	void addOrUpdate(const std::string& path);
	const std::vector<Entry>& getEntries() const{return entries;}

private:
	std::string dbPath;
	std::vector<Entry> entries;
	Entry* findEntry(const std::string& path);
};
