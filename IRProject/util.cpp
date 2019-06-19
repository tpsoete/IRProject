#include "util.h"


// import the Porter stemmer
extern "C" {
	int stem(char * p, int i, int j);
}

void word_stem(char* s)
{
	return;
	int i, l = (int)strlen(s);
	// stem function requires lowercase letters
	for (i = 0; i < l; i++) s[i] = tolower(s[i]);
	// use the stem function above
	l = stem(s, 0, l - 1) + 1;
	// manually add the end sign
	s[l] = '\0';
}

std::string word_stem(const std::string & s)
{
	return s;
	char* buf = new char[s.length() + 1];
	strcpy(buf, s.c_str());
	word_stem(buf);
	std::string ans(buf);
	delete[] buf;
	return ans;
}

std::string reuters(int i)
{
	static char s[30];
	sprintf(s, "Reuters/%d.html", i);
	return s;
}

std::vector<std::string> split(const std::string & s)
{
	std::vector<std::string> ans;
	size_t i = 0, j, n = s.length();
	for (;;) {
		while (i < n && s[i] == ' ') ++i;
		j = i;
		while (j < n && s[j] != ' ') ++j;
		ans.emplace_back(s.substr(i, j - i));
		if (j == n) break;
		i = j;
	}
	return ans;
}
