#include "PostingList.h"



PostingList::PostingList()
{
	frequency = 0;
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

size_t PostingList::get_tf(size_t doc_id)
{
	for (int i = 0; i < list.size(); i++) {
		if (list[i].docID==doc_id) {
			return list[i].positions.size();
		}
	}
	return 0;
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
