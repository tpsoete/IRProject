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
std::vector<size_t> InvertedIndex::Phrase(std::vector<std::string> word_input)//短语查询
{
	std::vector<size_t>res;
	int length = word_input.size();
	if (length == 0) {
		std::cout << "no input" << "\n";
		return std::vector<size_t>();//长度为0
	}
	int now_index = 0;//p1
	int index_p1, index_p2;
	PostingList list_p1, list_p2;
	auto flag = dictionary.find(word_input[0]);
	if (flag == dictionary.end()) {
		return std::vector<size_t>();
	}
	list_p1 = dictionary.find(word_input[0])->second;
	while (length > now_index+1)
	{
		index_p2 = now_index + 1;
		auto flag = dictionary.find(word_input[index_p2]);
		if (flag == dictionary.end()) {
			return std::vector<size_t>();
		}
		list_p2 = dictionary.find(word_input[index_p2])->second;
		list_p1 = Intersect(list_p1, list_p2, 1);
		now_index++;
	}
	for (int i = 0; i < list_p1.list.size(); i++)
	{
		res.push_back(list_p1.list[i].docID);
	}
	if (list_p1.list.size()==0)
	{
		std::cout << "not found";
	}
	list_p1.Output();
	return res;//输出list_p1的转化项
}

PostingList InvertedIndex::Intersect(PostingList p1, PostingList p2, int k)//位置索引的合并算法
{
	PostingList res;
	int length_p1 = p1.list.size();
	int length_p2 = p2.list.size();
	int now_p1=0, now_p2=0;
	int pos_p1, pos_p2, pos_ind_p1, pos_ind_p2;
	IndexNode temp_list;
	while (now_p1<length_p1&&now_p2<length_p2)
	{
		if (p1.list[now_p1].docID == p2.list[now_p2].docID) {
			temp_list.positions.clear();
			temp_list.docID = p1.list[now_p1].docID;
			pos_ind_p1 = 0;
			pos_ind_p2 = 0;
			pos_p1 = p1.list[now_p1].positions[pos_ind_p1];
			pos_p2 = p2.list[now_p2].positions[pos_ind_p2];
			while (pos_ind_p1<p1.list[now_p1].positions.size())
			{
				pos_p1 = p1.list[now_p1].positions[pos_ind_p1];
				while (pos_ind_p2 < p2.list[now_p2].positions.size())
				{
					pos_p2 = p2.list[now_p2].positions[pos_ind_p2];
					if (pos_p2-pos_p1==k)
					{
						temp_list.positions.push_back(pos_p2);
					}
					else
					{
						if (pos_p2>pos_p1)
						{
							break;
						}
					}
					pos_ind_p2++;
				}
				pos_ind_p1++;
			}
			if (temp_list.positions.size()!=0)
				res.list.push_back(temp_list);
			now_p1++;
			now_p2++;
		}
		else
		{
			if (p1.list[now_p1].docID < p2.list[now_p2].docID)
			{
				now_p1++;
			}
			else {
				now_p2++;
			}
		}
	}
	return res;
}


void InvertedIndex::Output() const
{
	for (auto& p : dictionary) {
		p.second.Output(p.first);
	}
}
