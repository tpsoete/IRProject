#pragma once
#include "util.h"
#include <map>
#include <set>

class PermutermIndex
{
public:
	std::map<std::string, std::string> index;

	std::string Add(const std::string& word);	// return the stemmed word by the way
	std::set<std::string> FuzzySearch(const std::string& pattern) const;
	
	void Output() const;
};

