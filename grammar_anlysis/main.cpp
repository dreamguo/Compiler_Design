#define _CRT_SECURE_NO_WARNINGS
#include "myFunction.h"

list<int> LIST; 
queue<string> q;
set<string> names;

void grammarAnalysis(FILE* fi, ofstream &fo);

void error(int a) {
	cout << "something wrong in " << a << endl;
	exit(1);
}

int main() {
	FILE* fi;
	if ((fi = fopen("testfile.txt", "r")) == NULL) {
		printf("Open input file error\n");
		exit(1);
	}
	ofstream fo("output.txt", ios::out);
	
	grammarAnalysis(fi, fo);

	fclose(fi);
	fo.close();
	return 0;
}
