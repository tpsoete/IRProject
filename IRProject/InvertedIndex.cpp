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
		// printf("%s does not exist!\n", filename.c_str());
		return;
	}
	size_t docn = docName.size();
	// allocate memory for filenames and assign a new id for the document
	docName.push_back(filename);

	char c[256];
	// word counter
	size_t cnt = 0;
	// INDEX GENERATION
	while (f.GetWord(c)) {
		for (int i = 0; c[i]; i++) c[i] = tolower(c[i]);
		// add into the permuterm index and stem the word (in permuterms.add())
		std::string cs;
		if (bWildcard) {
			cs = permuterms.Add(c);
		}
		else {
			word_stem(c);
			cs = c;
		}
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

std::vector<size_t> InvertedIndex::TopK(const std::vector<std::string>& words, int k)
{
	std::vector<size_t> ans;
	std::vector<doc_with_score> heap;
	heap = get_scores(words);
	for (int n = 0; n < k; n++) {
		size_t CurrentSize = heap.size();
		for (int i = CurrentSize / 2 - 1; i >= 0; i--)
		{
			doc_with_score temp = heap[i];
			int c = 2 * i + 1;
			while (c <= CurrentSize)
			{
				if (c < CurrentSize && heap[c].score < heap[c + 1].score)
					c++;
				if (temp.score >= heap[c].score)
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

std::vector<doc_with_score> InvertedIndex::get_scores(const std::vector<std::string>& words)
{
	std::vector<doc_with_score> scores;
	const size_t N = docName.size();
	const size_t M = words.size();
	std::vector<std::vector<double>> W;//tf-idf vector space, M-N+1
	//get the vector space
	std::vector<double> w;
	for (int i = 0; i < M; i++) {
		auto List = dictionary.find(words[i])->second;
		size_t df = List.list.size();
		for (int j = 0; j < N; j++) {
			size_t tf = List.get_tf(j);
			if(tf==0){
				w.push_back(0);
			}
			else{
				w.push_back((1 + log(tf))*log(N / df));
			}
		}
		w.push_back((1 + log(1))*log(N / df));//tf-idf of the searching words
		W.push_back(w);
		w.clear();
	}
	//normalize the score
	double score = 0;
	double mold_n = 0;
	for (int j = 0; j < M; j++) {
		mold_n += W[j][N] * W[j][N];
	}
	mold_n = sqrt(mold_n);
	//get cosine score of all the document with the searching words
	for (int i = 0; i < N; i++) {
		double score=0;
		double mold = 0;
		for (int j = 0; j < M; j++) {
			score += W[j][i] * W[j][N];
			mold += W[j][i] * W[j][i];
		}
		mold = sqrt(mold);
		doc_with_score s;
		s.doc_id = i;
		s.score = score/(mold*mold_n);
		scores.push_back(s);
	}
	return scores;
}

void InvertedIndex::Output() const
{
	for (auto& p : dictionary) {
		p.second.Output(p.first);
	}
}
