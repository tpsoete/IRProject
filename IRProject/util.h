#pragma once

#include<string>
#include<vector>
#include<iostream>

#ifdef _MSC_VER
#pragma warning(disable:4996)
#endif

void word_stem(char* s);
std::string word_stem(const std::string& s);

std::string reuters(int i);
std::vector<std::string> split(const std::string& s);
