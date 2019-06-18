#pragma once

#include "util.h"

struct IndexNode
{
	size_t docID;
	std::vector<size_t> positions;	// sorted word positions in the document, its size is frequency (tf)
};

class PostingList
{
public:
	std::vector<IndexNode> list;	// all the document the word exists, its size is document frequency (df)
	size_t frequency;	// collection frequency (cf)

	PostingList();

	void Add(size_t docID, size_t position);	// should only be used in InvertedIndex::AddFile

	void Output(std::string word = "") const;	// Debug output
};

