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
    
    void AddFile(std::string filename);
    std::vector<size_t> Search(std::string word);
    std::vector<size_t> TopK(std::string words[], int k);

	void Output() const;
};

