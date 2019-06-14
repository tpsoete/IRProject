#pragma once

#include "util.h"

struct IndexNode
{
	size_t docID;
	std::vector<size_t> positions;	// sorted word positions in the document, its size is frequency
};

class PostingList
{
public:
	std::vector<IndexNode> list;
	size_t frequency;

	PostingList();
	~PostingList();

	void Add(size_t docID, size_t position);

	void Output(std::string word = "") const;
};

