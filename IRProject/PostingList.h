#pragma once

#include "util.h"

struct IndexNode
{
	size_t docID;
	std::vector<size_t> position;	// sorted word positions in the document, its size is frequency
};

class PostingList
{
public:
	std::vector<IndexNode> list;
	size_t frequency;
	bool stop;		// whether the word is a stop word

	PostingList();
	~PostingList();
};

