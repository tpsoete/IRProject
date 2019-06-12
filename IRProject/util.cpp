#include "util.h"


// import the Porter stemmer
extern "C" {
	int stem(char * p, int i, int j);
}

void word_stem(char* s)
{
	int i, l = (int)strlen(s);
	// stem function requires lowercase letters
	for (i = 0; i < l; i++) s[i] = tolower(s[i]);
	// use the stem function above
	l = stem(s, 0, l - 1) + 1;
	// manually add the end sign
	s[l] = '\0';
}

std::string reuters(int i)
{
	static char s[30];
	sprintf(s, "Reuters\\%d.html", i);
	return s;
}
