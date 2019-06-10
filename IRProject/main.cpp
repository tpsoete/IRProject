#include "InvertedIndex.h"

using std::cin;
using std::cout;

void help()
{
	// hint info
	puts("exit - exit this program");
	puts("help - print this help");
}

InvertedIndex index;

bool console()
{
	std::string input;
	cout << ">>>";
	cin >> input;
	if (input == "exit") return false;
	else if (input == "help") help();
	// else ...

	return true;
}

int main()
{
	help();
	while (console());
	system("pause");
	return 0;
}