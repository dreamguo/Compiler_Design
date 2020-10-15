#define _CRT_SECURE_NO_WARNINGS
#include "my.h"

list<int> LIST;
queue<string> q;
set<string> names;
map<string, list<int>> global_labels;
map<string, list<int>> local_labels;
int LINE_NUM = 1;
int LINE_NUM_LAST = 1;
int isLocalFunc = 0; // 0=全局, 1=void func, 2=int func, 3=char func
FILE* fi;
ofstream fo("output.txt", ios::out);
ofstream fe("error.txt", ios::out);

void tofe(string str) {
	fe << str << endl;
	// cout << str << endl;
}

void tofo(string str) {
	// fo << str << endl;
	cout << str << endl;
}

void error(int a, char type, int line_num) {
	if (line_num <= 0) {
		line_num = LINE_NUM;
	}
	cout << "something wrong in " << a << " LINE: " << line_num << endl;
	if (type != '-') {
		string str = to_string(line_num) + " " + type;
		tofe(str);
	}
	// exit(1);
}

int main() {
	if ((fi = fopen("testfile.txt", "r")) == NULL) {
		printf("Open input file error\n");
		exit(1);
	}

	grammarAnalysis();
	
	FILE* fee;
	if ((fee = fopen("error.txt", "r")) == NULL) {
		printf("Open input file error\n");
		exit(1);
	}
	char sym;
	int sum = 0;
	while ((sym = fgetc(fee)) != ' ') {
		sum = sum * 10 + sym - '0';
	}
	//assert(sum < 27);
	sym = fgetc(fee);
	// assert(sym == 'i');
	
	fclose(fi);
	fclose(fee);
	fo.close();
	fe.close();
	return 0;
}
