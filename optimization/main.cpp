#define _CRT_SECURE_NO_WARNINGS
#include "my.h"

list<int> LIST;
queue<string> q;
map<string, list<int>> global_labels;
map<string, list<int>> local_labels;
int LINE_NUM = 1;
int LINE_NUM_LAST = 1;
int isLocalFunc = 0; // 0=全局, 1=void func, 2=int func, 3=char func
int IS_ANY_ERROR = 0;  // 0=无错, 1=有错(a-o,其余错不管)  若不加,则会崩溃(比如a['c'],在stoi('c')的时候崩溃)
FILE* fi;
list<Siyuanshi> siyuanshi_list;
list<Siyuanshi> siyuanshi_data_list;

ofstream fo("output.txt", ios::out);
ofstream fe("error.txt", ios::out);
ofstream fmiddle("middle.txt", ios::out);
ofstream fmips("mips.txt", ios::out);
ofstream f_before("17231051_郭梦琦_优化前中间代码.txt", ios::out);
ofstream f_after("17231051_郭梦琦_优化后中间代码.txt", ios::out);

void tomiddle(string str) {
	fmiddle << str << endl;
	if (DEBUG_MIDDLE == 1) {
		cout << str << endl;
	}
}

void tofmips(string str) {
	fmips << str << endl;
	if (DEBUG_MIPS == 1) {
		cout << str << endl;
	}
}

void tofe(string str) {
	IS_ANY_ERROR = 1;
	if (DEBUG_ERROR == 1) {
		fe << str << endl;
		cout << str << endl;
		cout << "!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!" << endl;
	}
}

void tofo(string str) {
	if (DEBUG_MIDDLE == 1) {
		fo << str << endl;
		cout << str << endl;
	}
}

void error(int a, char type, int line_num) {
	if (line_num <= 0) {
		line_num = LINE_NUM;
	}
	if (DEBUG_ERROR == 1) {
		cout << "something wrong in " << a << " LINE: " << line_num << endl;
	}
	if (type != '-') {
		string str = to_string(line_num) + " " + type;
		tofe(str);
	}
	// exit(1);
}

void middle_before_optimal() {
	for (list<Siyuanshi>::reverse_iterator iter = siyuanshi_data_list.rbegin();
		iter != siyuanshi_data_list.rend(); iter++) {
		f_before << (*iter).output_string() << endl;
	}
	for (list<Siyuanshi>::reverse_iterator iter = siyuanshi_list.rbegin(); 
		iter != siyuanshi_list.rend(); iter++) {
		f_before << (*iter).output_string() << endl;
	}
}

void middle_after_optimal() {
	for (list<Siyuanshi>::reverse_iterator iter = siyuanshi_data_list.rbegin();
		iter != siyuanshi_data_list.rend(); iter++) {
		f_after << (*iter).output_string() << endl;
	}
	for (list<Siyuanshi>::reverse_iterator iter = siyuanshi_list.rbegin();
		iter != siyuanshi_list.rend(); iter++) {
		f_after << (*iter).output_string() << endl;
	}
}

int main() {
	if ((fi = fopen("testfile.txt", "r")) == NULL) {
		cout << "Open input file error\n" << endl;
		exit(1);
	}
	grammarAnalysis();
	if (IS_ANY_ERROR == 0) {
		middle_before_optimal();
		optimal(); 
		middle_after_optimal(); 
		generate_mips();
	}

	fclose(fi);
	fo.close();
	fe.close();
	fmiddle.close();
	fmips.close();
	f_before.close();
	f_after.close();
	return 0;
}
