#include "InvertedIndex.h"
#include "TextFile.h"


InvertedIndex::InvertedIndex()
{
}


InvertedIndex::~InvertedIndex()
{
}

bool InvertedIndex::AddFile(std::string filename)
{
	TextFile f(filename.c_str());
	// the file is not exist
	if (f.file == NULL) {
		// printf("%s does not exist!\n", filename.c_str());
		return false;
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
	docLength.push_back(cnt);
	return true;
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

std::vector<doc_with_score> InvertedIndex::TopK(const std::vector<std::string>& words, int k)
{
	std::vector<doc_with_score> ans;
	std::vector<doc_with_score> heap;
	heap = get_scores(words);
	for (int n = 0; n < k; n++) {
		size_t CurrentSize = heap.size();
		for (int i = CurrentSize / 2 - 1; i >= 0; i--)
		{
			doc_with_score temp = heap[i];
			int c = 2 * i + 1;
			while (c < CurrentSize)
			{
				if (c < CurrentSize - 1 && heap[c].score < heap[c + 1].score)
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
		ans.push_back(heap[0]);
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
			if (tf == 0 || df == 0 || df == N) {
				w.push_back(0);
			}
			else {
				w.push_back((1 + log10(tf))*log10(N / df));
			}
		}
		//tf-idf of the searching words
		if (df == 0 || df == N)
			w.push_back(0);
		else
			w.push_back((1 + log10(1))*log10(N / df));
		W.push_back(w);
		w.clear();
	}
	//normalize the score
	double score = 0;
	//get cosine score of all the document with the searching words
	for (int i = 0; i < N; i++) {
		double score = 0;
		for (int j = 0; j < M; j++) {
			score += W[j][i] * W[j][N];
		}
		doc_with_score s;
		s.doc_id = i;
		s.score = score/docLength[i];
		scores.push_back(s);
	}
	return scores;
}


/* The following is added by Span
 * Boolean search, cmd is what user puts in
 * the function will check whether the input is legal
 * does not support brackets
 * for example: "a AND b OR NOT c"
 */
std::vector<size_t> InvertedIndex::Boolean_serach(std::string cmd) {
	std::stringstream iss(cmd);
	std::string token_item;
	std::vector<std::string> tokens;
	std::vector<size_t> result;

	int op_flag = 2;
	//0 for waiting for 'AND' or 'OR', 1 for 'AND' and 2 for 'OR'
	bool not_flag = false;

	//the following is to split the command into tokens.
	while (std::getline(iss, token_item, ' ')) {
		for (auto c : token_item) {
			if (c == ' ') {
				continue;
			}
		}
		if (token_item.size() > 0)
			tokens.push_back(word_stem(token_item));
	}


	for (auto tok : tokens) {
		if (stricmp(tok.c_str(), "and") == 0) {
			if (op_flag == 1 || op_flag == 2) {
				std::cout << "error syntax in boolean search£¡" << std::endl;
				exit(1);
			}
			else {
				op_flag = 1;
				continue;
			}
		}
		else if (stricmp(tok.c_str(), "or") == 0) {
			if (op_flag == 1 || op_flag == 2) {
				std::cout << "error syntax in boolean search£¡" << std::endl;
				exit(1);
			}
			else {
				op_flag = 2;
				continue;
			}
		}
		else if (stricmp(tok.c_str(), "not") == 0) {
			if (op_flag == 0) {
				std::cout << "error syntax in boolean search£¡ " << std::endl;
				exit(1);
			}
			else {
				not_flag = not(not_flag);
			}
		}
		else {
			std::vector<size_t> tmp = Search(tok);
			std::vector<size_t> new_result;
			sort(tmp.begin(), tmp.end());
			if (not_flag) {
				size_t i = 0;
				std::vector<size_t> not_tmp;
				int index = 0;
				while (i < docName.size()) {
					if (i < tmp[index]) {
						not_tmp.push_back(i);
						++i;
					}
					else {
						++index;
						if (index >= tmp.size()) {
							for (; i < docName.size(); ++i) {
								not_tmp.push_back(i);
							}
							break;
						}
					}
				}
				tmp = not_tmp;
			}
			int index1 = 0, index2 = 0;
			switch (op_flag) {
			case 0:
				std::cout << "error syntax in boolean search£¡ " << std::endl;
				break;
			case 1: //case :AND
				while (index1 < result.size() && index2 < tmp.size()) {
					if (result[index1] < tmp[index2]) {
						++index1;
					}
					else if (result[index1] == tmp[index2]) {
						new_result.push_back(result[index1]);
						++index1;
						++index2;
					}
					else if (result[index1] > tmp[index2]) {
						++index2;
					}
				}
				break;
			case 2: //case : OR
				while (index1 < result.size() && index2 < tmp.size()) {
					if (result[index1] < tmp[index2]) {
						new_result.push_back(result[index1]);
						++index1;
					}
					else if (result[index1] == tmp[index2]) {
						new_result.push_back(result[index1]);
						++index1;
						++index2;
					}
					else if (result[index1] > tmp[index2]) {
						new_result.push_back(tmp[index2]);
						++index2;
					}
				}
				while (index1 < result.size()) {
					new_result.push_back(result[index1]);
					++index1;
				}
				while (index2 < tmp.size()) {
					new_result.push_back(tmp[index2]);
					++index2;
				}
				break;
			}
			not_flag = 0;
			op_flag = 0;
			result = new_result;
		}
	}
	return result;
}


std::string InvertedIndex::Spell_correction(std::string source) {
	std::vector<std::string> all_words;
	std::vector<int> scores;
	int min_index = 0;
	if (source.length() == 0)
		return "";
	auto it = dictionary.begin();
	while (it != dictionary.end()) {
		auto target = it->first;
		int n = source.length();
		int m = target.length();
		std::vector<std::vector<int>> matrix(n + 1);
		for (int i = 0; i <= n; i++) matrix[i].resize(m + 1);
		for (int i = 1; i <= n; i++) matrix[i][0] = i;
		for (int i = 1; i <= m; i++) matrix[0][i] = i;
		for (int i = 1; i <= n; i++) {
			const char si = source[i - 1];
			for (int j = 1; j <= m; j++) {
				const char dj = target[j - 1];
				int cost;
				if (si == dj) {
					cost = 0;
				}
				else {
					cost = 1;
				}
				const int above = matrix[i - 1][j] + 1;
				const int left = matrix[i][j - 1] + 1;
				const int diag = matrix[i - 1][j - 1] + cost;
				matrix[i][j] = std::min(above, std::min(left, diag));
			}
		}
		scores.push_back(matrix[n][m]);
		all_words.push_back(target);

	}
	for (int i = 0; i < scores.size(); ++i) {
		if (scores[min_index] >= scores[i]) {
			min_index = i;
		}
	}
	return all_words[min_index];

}

void InvertedIndex::Output() const
{
	for (auto& p : dictionary) {
		p.second.Output(p.first);
	}
}
