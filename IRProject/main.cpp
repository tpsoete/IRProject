#include "InvertedIndex.h"
#include <functional>

using namespace std;

InvertedIndex idx;
PermutermIndex pmx;

map<string, function<void()> > cmd;
void bind_command()
{
	static char buf[256];
	static bool all = false;

	// initialize

	idx.bWildcard = false;

    // connect input strings with functions(lambda expression)
	// format: cmd["command"] = []() { /* what to do */ };

	cmd["help"] = []() {	
		// hint info
		puts("exit - exit this program");
		puts("help - print this help");
		puts("");
		puts("stem %s - word stemming"); 
		puts("search %s - simple search, return the posting list");
		puts("add %d - add a Reuter document");
		puts("wildcard %s - wildcard search, only return unstemmed words for simplicity");
		puts("topk %d %s %s ... - TopK search, the first parameter is k. e.g. 'topk 3 how much'");
	};

	cmd["stem"] = []() {
		scanf("%s", buf);
		word_stem(buf);
		puts(buf);
	};

	cmd["search"] = []() {
		string s;
		cin >> s;
		auto v = idx.Search(s);
		if (v.empty()) puts("No result!");
	};

	cmd["add"] = []() {
		int i;
		cin >> i;
		idx.AddFile(reuters(i));
	};

	cmd["addall"] = []() {
		if (all) return;
		for (int i = 1; i <= 21576; i++) {
			idx.AddFile(reuters(i));
			if (i % 1000 == 0) printf("%d\n", i);
		}
		all = true;
	};

	cmd["wildcard"] = []() {
		string s;
		cin >> s;
		auto ans = idx.permuterms.Wildcard(s);
		for (auto& str : ans) cout << str << endl;
	};

	cmd["topk"] = []() {
		int k;
		string s;
		vector<string> words;
		cin >> k;
		if (k <= 1) {
			puts("invalid k");
			return;
		}
		getline(cin, s);
		cin.unget();
		words = split(s);
		for (auto& str : words) str = word_stem(str);
		auto ans = idx.TopK(words,k);
		for (auto x : ans) {
			printf("%20s%20f\n", idx.docName[x.doc_id].c_str(), x.score);
		}
	};

	// debug

	cmd["check"] = []() {
		idx.Output();
	};

	cmd["names"] = []() {
		puts("docID\tName");
		for (size_t i = 0; i < idx.docName.size(); i++)
			printf("%zd\t%s\n", i, idx.docName[i].c_str());
	};

	// test for permuterm index

	cmd["permadd"] = []() {
		string s;
		cin >> s;
		pmx.Add(s);
	};

	cmd["perm"] = []() {
		string s;
		cin >> s;
		auto ans = pmx.Wildcard(s);
		for (auto& str : ans) cout << str << endl;
	};

	cmd["permlist"] = []() {
		pmx.Output();
	};

	cmd["perminit"] = []() {
		pmx.Add("this");
		pmx.Add("that");
		pmx.Add("thus");
	};
}

int main()
{
    string input;
    bind_command();
	cmd["help"]();
    for(;;) {
        cout << ">>> ";
        cin >> input;
        if(input == "exit") break;
        auto it = cmd.find(input);
        if(it == cmd.end()) {
            cerr << "Unknown command " << input << endl;
			cmd["help"]();
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
