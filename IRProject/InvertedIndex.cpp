#include "InvertedIndex.h"
#include "TextFile.h"


InvertedIndex::InvertedIndex()
{
}


InvertedIndex::~InvertedIndex()
{
}

void InvertedIndex::AddFile(std::string filename)
{
	TextFile f(filename.c_str());
	// the file is not exist
	if (f.file == NULL) {
		printf("%s does not exist!\n", filename.c_str());
		return;
	}
	size_t l = filename.length(), docn = docName.size();
	// allocate memory for filenames and assign a new id for the document
	docName.push_back(filename);

	char c[256];
	// word counter
	size_t cnt = 0;
	// INDEX GENERATION
	while (f.GetWord(c)) {
		for (int i = 0; c[i]; i++) c[i] = tolower(c[i]);
		// add into the permuterm index and stem the word (in permuterms.add())
		std::string cs = permuterms.Add(c);
		// find the word and insert if unfound
		auto it = dictionary.find(cs);
		if (it == dictionary.end()) it = dictionary.emplace(cs, PostingList()).first;
		PostingList& p = it->second;
		// store the index
		p.Add(docn, ++cnt);
	}
}

std::vector<size_t> InvertedIndex::Search(std::string word)
{
	std::vector<size_t> ans;
	word = word_stem(word);
	auto it = dictionary.find(word);
	if (it == dictionary.end()) return ans;
	for (auto& node : it->second.list) ans.push_back(node.docID);
	it->second.Output(word);
	return ans;
}

std::vector<size_t> InvertedIndex::TopK(std::string words[], int k)
{
	struct doc_with_score {
		size_t doc_id;
		double scrore;
	};
	std::vector<size_t> ans;
	std::vector<doc_with_score> heap;
	for (int n = 0; n < k; n++) {
		size_t CurrentSize = heap.size();
		for (int i = CurrentSize / 2 - 1; i >= 0; i--)
		{
			doc_with_score temp = heap[i];
			int c = 2 * i + 1;
			while (c <= CurrentSize)
			{
				if (c < CurrentSize && heap[c].scrore < heap[c + 1].scrore)
					c++;
				if (temp.scrore >= heap[c].scrore)
					break;
				else
				{
					heap[(c - 1) / 2] = heap[c];
					c = 2 * c + 1;
				}        
			}
			heap[(c - 1) / 2] = temp;
		}
		ans.push_back(heap[0].doc_id);
		heap[0] = heap[CurrentSize - 1];
		heap.pop_back();
	}
    return ans;
}

void InvertedIndex::Output() const
{
	for (auto& p : dictionary) {
		p.second.Output(p.first);
	}
}
