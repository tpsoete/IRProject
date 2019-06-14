#include "PostingList.h"



PostingList::PostingList()
{
	frequency = 0;
}


PostingList::~PostingList()
{
}

void PostingList::Add(size_t docID, size_t position)
{
	if (!list.empty() && docID < list.back().docID) {
		puts("please add in increase order of docID");
		return;
	}
	if (list.empty() || docID > list.back().docID) {
		list.emplace_back();
		list.back().docID = docID;
	}
	list.back().positions.push_back(position);
	++frequency;
}

void PostingList::Output(std::string word) const
{
	if (word.length()) printf("WORD %s\n", word.c_str());
	printf("freq = %zd count = %zd\n", frequency, list.size());
	for (auto& node : list) {
		printf("\tdocID = %zd <%zd", node.docID, node.positions[0]);
		for (size_t i = 1; i < node.positions.size(); i++) printf(", %zd", node.positions[i]);
		puts(">;");
	}
}
