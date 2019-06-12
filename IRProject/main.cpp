#include "InvertedIndex.h"

using namespace std;

// commands

void help()
{
	// hint info
	puts("exit - exit this program");
	puts("help - print this help");
    
    puts("stem - word stemming");
}

void stem_test()
{
    static char buf[256];
    scanf("%s", buf);
    word_stem(buf);
    puts(buf);
}

// command handling

map<string, void(*)()> cmd;
void bind_command()
{
    // connect input strings with functions
    cmd["help"] = help;
    cmd["stem"] = stem_test;
}

int main()
{
    string input;
	help();
    bind_command();
    for(;;) {
        cout << ">>> ";
        cin >> input;
        if(input == "exit") break;
        auto it = cmd.find(input);
        if(it == cmd.end()) {
            cerr << "Unknown command " << input << endl;
            help();
        }
        else (it->second)();
        getline(cin, input);    // clear the rest input in the line
    }
    puts("");
#ifdef _MSC_VER
	system("pause");
#endif
	return 0;
}
