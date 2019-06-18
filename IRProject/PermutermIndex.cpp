#include "PermutermIndex.h"
#include <regex>

std::string PermutermIndex::Add(const std::string & word)
{
	std::string stem = word_stem(word);
	if (index.find(word + "$") != index.end()) return stem;

	size_t n = word.length();
	std::string w2 = word + "$" + word;
	for (size_t i = 0; i < n; i++) {
		std::string tmp = w2.substr(i, n + 1);
		index[tmp] = stem;
	}
	return stem;
}

static std::regex make_pattern(const std::string& pattern)
{
	std::string s = "";
	for (auto ch : pattern) {
		if (ch == '*') s += ".*";
		else s += ch;
	}
	return std::regex(s);
}

static std::string restore(const std::string& permuterm)
{
	int i, n = (int)permuterm.length();
	for (i = 0; i < n; i++) if (permuterm[i] == '$') break;
	if (i == n) return permuterm;
	return permuterm.substr(i + 1) + permuterm.substr(0, i);
}

std::set<std::string> PermutermIndex::FuzzySearch(const std::string & pattern) const
{
	std::set<std::string> ans;
	int i, n = (int)pattern.length();

	for (i = n - 1; i >= 0; i--) if (pattern[i] == '*') break;
	if (i < 0) {
		// no * inside
		auto it = index.find(pattern + "$");
		if (it != index.end()) ans.insert(it->second);
		return ans;
	}

	// move chars after * to front
	std::string query;
	if (i == n - 1) query = pattern;
	else query = pattern.substr(i + 1) + "$" + pattern.substr(0, i + 1);

	// remove chars after the first *
	for (i = 0; i < (int)query.length(); i++) if (query[i] == '*') break;
	query.erase(i, std::string::npos);

	printf("actual search %s*\n", query.c_str());
	auto lower = index.lower_bound(query);
	query.back()++;
	auto upper = index.upper_bound(query);

	std::regex re = make_pattern(pattern);
	for (auto it = lower; it != upper; ++it) {
		std::string origin = restore(it->first);
		if (std::regex_match(origin, re)) {
			ans.insert(it->second);
			printf("=%s\n", origin.c_str());
		}
	}
	return ans;
}

void PermutermIndex::Output() const
{
	for (auto& p : index) {
		printf("%s -> %s\n", p.first.c_str(), p.second.c_str());
	}
}