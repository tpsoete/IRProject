#pragma once

#include "PostingList.h"
#include "PermutermIndex.h"

class InvertedIndex
{
public:
	std::vector<std::string> docName;	// docID and corresponding document name
	std::map<std::string, PostingList> dictionary;
	PermutermIndex permuterms;

	InvertedIndex();
	~InvertedIndex();
    
    void AddFile(std::string filename);		// add a text file into the index
    std::vector<size_t> Search(std::string word);	// simple search for a single word
    std::vector<size_t> TopK(std::string words[], int k);

	void Output() const;
};

