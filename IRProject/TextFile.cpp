#include "TextFile.h"
#include "util.h"
#include <cctype>

TextFile::TextFile(const char* filename)
{
	file = fopen(filename, "r");
}

bool TextFile::GetWord(char* s)
{
	// the file isn't opened successfully
	if (file == NULL) return false;
	int c, i = 0;
	// find the first alpha
	do c = fgetc(file); while (c != EOF && !isalpha(c));
	// no more words
	if (c == EOF) return false;
	do {
		// add the valid char into the string
		s[i++] = c;
		// getchar from a file
		c = fgetc(file);
		// if the word may be broken into two lines
		if (c == '-') {
			c = fgetc(file);
			// check if the '-' is followed by a new line
			if (c == '\n') {
				// ignore the - and \n and concat two parts
				c = fgetc(file);
				if (isalpha(c)) continue;
			}
			// the next char maybe an valid letter, so put it back to the file
			else ungetc(c, file);
			break;
		}
		// the process end when read an invalid letter or EOF
	} while (c != EOF && isalpha(c));
	// put an end sign
	s[i] = '\0';
	// successfully get a word
	return true;
}
