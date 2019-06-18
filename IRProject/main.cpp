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
		puts("");
		puts("exit - exit this program");
		puts("help - print this help");
		puts("");
		puts("add %d - add a Reuter document");
		puts("addall - add all document, very slow");
		puts("adduntil %d - add all documents before the number");
		puts("search %s - simple search, return the posting list");
		puts("");
		puts("wildcard %s - wildcard search, only return unstemmed words for simplicity");
		puts("topk %d %s %s ... - TopK search, the first parameter is k. e.g. 'topk 3 how much'");
		puts("boolean <boolean expression> - boolean search");
		puts("");
		puts("stem %s - word stemming");
		puts("names - all loaded documents");
		puts("");
	};

	cmd["search"] = []() {
		string s;
		cin >> s;
		auto v = idx.Search(s);
		if (v.empty()) puts("No result!");
		for (auto id : v) cout << idx.docName[id] << endl;
		printf("Found %zd document(s)\n", v.size());
	};

	cmd["add"] = []() {
		int i;
		cin >> i;
		if (!idx.AddFile(reuters(i))) 
			puts("No such file!");
	};

	cmd["addall"] = []() {
		if (all) return;
		for (int i = 1; i <= 21576; i++) {
			idx.AddFile(reuters(i));
			if (i % 1000 == 0) printf("%d\n", i);
		}
		all = true;
	};

	cmd["adduntil"] = []() {
		if (all) return;
		int n;
		cin >> n;
		for (int i = 1; i <= n; i++) {
			idx.AddFile(reuters(i));
			if (i % 200 == 0) printf("%d\n", i);
		}
		all = true;
	};

	// search

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

	cmd["boolean"] = [] {
		string s;
		getline(cin, s);
		cin.unget();
		auto ans = idx.Boolean_serach(s);
		for (auto id : ans) cout << idx.docName[id] << endl;
		printf("Found %zd document(s)\n", ans.size());
	};

	// debug

	cmd["stem"] = []() {
		scanf("%s", buf);
		word_stem(buf);
		puts(buf);
	};

	cmd["names"] = []() {
		puts("docID\tName");
		for (size_t i = 0; i < idx.docName.size(); i++)
			printf("%zd\t%s\n", i, idx.docName[i].c_str());
	};

	cmd["dict"] = []() {
		for (auto& p : idx.dictionary) {
			puts(p.first.c_str());
		}
	};
	cmd["phrase"] = []() {
		string s;
		getline(cin, s);
		vector<std::string>words;
		words = split(s);
		for (auto& str : words) str = word_stem(str);
		idx.Phrase(words);
	};
	
	/*cmd["xbrdebug"] = []() {
		vector<std::string> words(2);
		words[0] = "crop";
		words[1] = "cocoa";
		for (auto& str : words) str = word_stem(str);
		idx.Phrase(words);
	};*/
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
