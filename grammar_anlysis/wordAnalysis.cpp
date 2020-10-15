#include "myFunction.h"

const int A_NUM = 10;

int isNumorWord(char a) {
	if (a >= '0' && a <= '9') {
		return 1;
	}
	if (a >= 'a' && a <= 'z') {
		return 1;
	}
	if (a >= 'A' && a <= 'Z') {
		return 1;
	}
	if (a == '_') {
		return 1;
	}
	return 0;
}

int analysis(FILE* fi, ofstream &fo) {
	char a[10];
	for (int i = 0; i < A_NUM; i++) {
		a[i] = ' ';
	}
	while ((a[0] = fgetc(fi)) != EOF) {
		switch (a[0])
		{
		case '+': {
			q.push("PLUS +");
			return 17;
		}
		case '-': {
			q.push("MINU -");
			return 18;
		}
		case '*': {
			q.push("MULT *");
			return 19;
		}
		case '/': {
			q.push("DIV /");
			return 20;
		}
		case '<': {
			if ((a[1] = fgetc(fi)) == '=') {
				q.push("LEQ <=");
				return 22;
			}
			else {
				q.push("LSS <");
				fseek(fi, -1, SEEK_CUR);
				return 21;
			}
		}
		case '>': {
			if ((a[1] = fgetc(fi)) == '=') {
				q.push("GEQ >=");
				return 24;
			}
			else {
				q.push("GRE >");
				fseek(fi, -1, SEEK_CUR);
				return 23;
			}
		}
		case '=': {
			if ((a[1] = fgetc(fi)) == '=') {
				q.push("EQL ==");
				return 25;
			}
			else {
				q.push("ASSIGN =");
				fseek(fi, -1, SEEK_CUR);
				return 27;
			}
		}
		case '!': {
			if ((a[1] = fgetc(fi)) == '=') {
				q.push("NEQ !=");
				return 26;
			}
			else {
				error(100);
			}
		}
		case ';': {
			q.push("SEMICN ;");
			return 28;
		}
		case ',': {
			q.push("COMMA ,");
			return 29;
		}
		case '(': {
			q.push("LPARENT (");
			return 30;
		}
		case ')': {
			q.push("RPARENT )");
			return 31;
		}
		case '[': {
			q.push("LBRACK [");
			return 32;
		}
		case ']': {
			q.push("RBRACK ]");
			return 33;
		}
		case '{': {
			q.push("LBRACE {");
			return 34;
		}
		case '}': {
			q.push("RBRACE }");
			return 35;
		}
		case '\'': {
			string str = "CHARCON ";
			a[1] = fgetc(fi);			// a[1]可以是'+'或'*'
			if (a[1] != '\'' && (a[2] = fgetc(fi)) == '\'') {
				str += a[1];
			}
			q.push(str);
			return 2;
		}
		case '"': {
			string str = "STRCON ";
			while ((a[1] = fgetc(fi)) != '"') {
				str += a[1];
			}
			q.push(str);
			return 3;
		}
		case '0':
		case '1':
		case '2':
		case '3':
		case '4':
		case '5':
		case '6':
		case '7':
		case '8':
		case '9': {
			string str = "INTCON ";
			str += a[0];
			while (((a[1] = fgetc(fi)) <= '9') && (a[1] >= '0')) {
				str += a[1];
			}
			if (a[1] != ' ') {
				fseek(fi, -1, SEEK_CUR);
			}
			q.push(str);
			return 1;
		}
		default:
			break;
		}
		if (isspace(a[0])) {
			continue;
		}
		if ((a[0] == EOF) | (!isNumorWord(a[0]))) {
			break;
		}
		for (int i = 1; i < A_NUM; i++) {
			a[i] = fgetc(fi);
			if (!isNumorWord(a[i])) {
				break;
			}
		}
		if (a[0] == 'c' && a[1] == 'o' && a[2] == 'n' && a[3] == 's' && a[4] == 't' && !isNumorWord(a[5])) {
			q.push("CONSTTK const");
			if (a[5] != ' ') {
				fseek(fi, -1, SEEK_CUR);
			}
			return 4;
		}
		else if (a[0] == 'i' && a[1] == 'n' && a[2] == 't' && !isNumorWord(a[3])) {
			q.push("INTTK int");
			if (a[3] != ' ') {
				fseek(fi, -1, SEEK_CUR);
			}
			return 5;
		}
		else if (a[0] == 'c' && a[1] == 'h' && a[2] == 'a' && a[3] == 'r' && !isNumorWord(a[4])) {
			q.push("CHARTK char");
			if (a[4] != ' ') {
				fseek(fi, -1, SEEK_CUR);
			}
			return 6;
		}
		else if (a[0] == 'v' && a[1] == 'o' && a[2] == 'i' && a[3] == 'd' && !isNumorWord(a[4])) {
			q.push("VOIDTK void");
			if (a[4] != ' ') {
				fseek(fi, -1, SEEK_CUR);
			}
			return 7;
		}
		else if (a[0] == 'm' && a[1] == 'a' && a[2] == 'i' && a[3] == 'n' && !isNumorWord(a[4])) {
			q.push("MAINTK main"); 
			if (a[4] != ' ') {
				fseek(fi, -1, SEEK_CUR);
			}
			return 8;
		}
		else if (a[0] == 'i' && a[1] == 'f' && !isNumorWord(a[2])) {
			q.push("IFTK if");
			if (a[2] != ' ') {
				fseek(fi, -1, SEEK_CUR);
			}
			return 9;
		}
		else if (a[0] == 'e' && a[1] == 'l' && a[2] == 's' && a[3] == 'e' && !isNumorWord(a[4])) {
			q.push("ELSETK else");
			if (a[4] != ' ') {
				fseek(fi, -1, SEEK_CUR);
			}
			return 10;
		}
		else if (a[0] == 'd' && a[1] == 'o' && !isNumorWord(a[2])) {
			q.push("DOTK do"); 
			if (a[2] != ' ') {
				fseek(fi, -1, SEEK_CUR);
			}
			return 11;
		}
		else if (a[0] == 'w' && a[1] == 'h' && a[2] == 'i' && a[3] == 'l' && a[4] == 'e' && !isNumorWord(a[5])) {
			q.push("WHILETK while"); 
			if (a[5] != ' ') {
				fseek(fi, -1, SEEK_CUR);
			}
			return 12;
		}
		else if (a[0] == 'f' && a[1] == 'o' && a[2] == 'r' && !isNumorWord(a[3])) {
			q.push("FORTK for");
			if (a[3] != ' ') {
				fseek(fi, -1, SEEK_CUR);
			}
			return 13;
		}
		else if (a[0] == 's' && a[1] == 'c' && a[2] == 'a' && a[3] == 'n' && a[4] == 'f' && !isNumorWord(a[5])) {
			q.push("SCANFTK scanf"); 
			if (a[5] != ' ') {
				fseek(fi, -1, SEEK_CUR);
			}
			return 14;
		}
		else if (a[0] == 'p' && a[1] == 'r' && a[2] == 'i' && a[3] == 'n' && a[4] == 't' && a[5] == 'f' && !isNumorWord(a[6])) {
			q.push("PRINTFTK printf");
			if (a[6] != ' ') {
				fseek(fi, -1, SEEK_CUR);
			}
			return 15;
		}
		else if (a[0] == 'r' && a[1] == 'e' && a[2] == 't' && a[3] == 'u' && a[4] == 'r' && a[5] == 'n' && !isNumorWord(a[6])) {
			q.push("RETURNTK return");
			if (a[6] != ' ') {
				fseek(fi, -1, SEEK_CUR);
			}
			return 16;
		}
		else {
			string str = "IDENFR ";
			int i = 0;
			while ((i < A_NUM) && isNumorWord(a[i])) {
				str += a[i++];
			}
			if (i == A_NUM) {
				a[0] = fgetc(fi);
				while (isNumorWord(a[0])) {
					str += a[0];
					a[0] = fgetc(fi);
				}
				if (a[0] != ' ') {
					fseek(fi, -1, SEEK_CUR);
				}
			}
			else {
				if (a[i] != ' ') {
					fseek(fi, -1, SEEK_CUR);
				}
			}
			q.push(str);
			return 0;
		}
	}
	return -1;
}

int wordAnalysis(FILE* fi, ofstream& fo, int force) {
	int sym;
	if (LIST.size() != q.size()) {		// check size
		cout << "LIST != q, LIST.size = " << LIST.size() << ", q.size() = " << q.size() << endl;
	}
	if (force == 1) {
		sym = analysis(fi, fo);
		LIST.push_back(sym);
		//cout << "force: " << sym << endl;
	}
	else {
		if (LIST.empty()) {
			sym = analysis(fi, fo);
			LIST.push_back(sym);
		}
		else {
			sym = LIST.front();
		}
	}
	return sym;
}