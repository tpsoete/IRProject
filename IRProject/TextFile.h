#pragma once

#include <cstdio>

class TextFile
{
public:
	// file pointer
	FILE* file;

	TextFile(const char* filename);
	bool GetWord(char* w);

	~TextFile() { if (file) fclose(file); }
};
