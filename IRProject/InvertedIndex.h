#pragma once

#include "PostingList.h"
#include "PermutermIndex.h"
#include <algorithm>
#include <sstream>
#include <unordered_map>

class InvertedIndex
{
public:
	std::vector<std::string> docName;	// docID and corresponding document name
	std::vector<size_t> docLength;	// how many words in the document
	std::unordered_map<std::string, PostingList> dictionary;
	PermutermIndex permuterms;

	bool bWildcard;		// enable wildcard search

	InvertedIndex();
	~InvertedIndex();
    
    bool AddFile(std::string filename);		// add a text file into the index
    std::vector<size_t> Search(std::string word);	// simple search for a single word
    std::vector<doc_with_score> TopK(const std::vector<std::string>& words, int k);
    std::vector<doc_with_score> get_scores(const std::vector<std::string>& words);

	std::vector<size_t> Boolean_serach(std::string cmd); //boolean search
	std::string Spell_correction(std::string error_word); //spell correction
	std::vector<size_t> DocIDList_and(std::vector<size_t> list1, std::vector<size_t> list2);
	std::vector<size_t> DocIDList_or(std::vector<size_t> list1, std::vector<size_t> list2);
	std::vector<size_t> DocIDList_not(std::vector<size_t> list);

	std::vector<size_t>Phrase(std::vector<std::string>word_input);//短语查询
	PostingList Intersect(PostingList p1, PostingList p2, int k);//基于位置索引的合并算法
	void Output() const;
};

