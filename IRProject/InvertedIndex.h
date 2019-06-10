#pragma once

#include "PostingList.h"
#include<map>

class InvertedIndex
{
public:
	std::vector<std::string> docName;
	std::map<std::string, PostingList> dictionary;

	InvertedIndex();
	~InvertedIndex();
};

