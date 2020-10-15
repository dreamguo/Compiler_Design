#include "myFunction.h"

int isChangLiangShuoMing(FILE* fi, ofstream& fo, int sym);
int isChangLiangDingYi(FILE* fi, ofstream &fo, int sym);
int isWuFuHaoZhengShu(FILE* fi, ofstream& fo, int sym, int more_than_0 = 0);
int isZhengShu(FILE* fi, ofstream& fo, int sym);
int isShengMingTouBu(FILE* fi, ofstream& fo, int sym);
int isBianLiangShuoMing(FILE* fi, ofstream& fo, int sym);
int isBianLiangDingYi(FILE* fi, ofstream& fo, int sym);
int isLeiXingBiaoShiFu(FILE* fi, ofstream& fo, int sym);
int isVoidHanShuDingYi(FILE* fi, ofstream& fo, int sym);
int isYouHanShuDingYi(FILE* fi, ofstream& fo, int sym); 
int isFuHeYuJu(FILE* fi, ofstream& fo, int sym);
int isCanShuBiao(FILE* fi, ofstream& fo, int sym);
int isZhuHanShu(FILE* fi, ofstream& fo, int sym);
int isBiaoDaShi(FILE* fi, ofstream& fo, int sym);
int isXiang(FILE* fi, ofstream& fo, int sym);
int isYinZi(FILE* fi, ofstream& fo, int sym);
int isYuJu(FILE* fi, ofstream& fo, int sym);
int isFuZhiYuJu(FILE* fi, ofstream& fo, int sym);
int isTiaoJianYuJu(FILE* fi, ofstream& fo, int sym);
int isTiaoJian(FILE* fi, ofstream& fo, int sym);
int isXunHuanYuJu(FILE* fi, ofstream& fo, int sym);
int isBuChang(FILE* fi, ofstream& fo, int sym);
int isHanShuDiaoYongYuJu(FILE* fi, ofstream& fo, int sym);
int isZhiCanShuBiao(FILE* fi, ofstream& fo, int sym);
int isYuJuLie(FILE* fi, ofstream& fo, int sym);
int isDuYuJu(FILE* fi, ofstream& fo, int sym);
int isXieYuJu(FILE* fi, ofstream& fo, int sym);
int isFanHuiYuJu(FILE* fi, ofstream& fo, int sym);


void judge(int sym, int a, ofstream& fo) {
	if (sym != a) {
		error(a);
	}
	LIST.pop_front();
	fo << q.front() << endl;
	if (sym == 3) {
		fo << "<字符串>" << endl;
	}
	cout << q.front() << endl;
	q.pop();
}

int isChangLiangShuoMing(FILE* fi, ofstream& fo, int sym) {
	while (sym == 4) {
		judge(sym, 4, fo);
		sym = isChangLiangDingYi(fi, fo, wordAnalysis(fi, fo));
		judge(sym, 28, fo);
		sym = wordAnalysis(fi, fo);
	}
	fo << "<常量说明>" << endl;
	return sym;
}

int isChangLiangDingYi(FILE* fi, ofstream &fo, int sym) {
	cout << "changliangdingyi: " << sym << endl;
	if (sym == 5) {
		judge(sym, 5, fo);
		sym = wordAnalysis(fi, fo);
		judge(sym, 0, fo);
		sym = wordAnalysis(fi, fo);
		judge(sym, 27, fo);
		sym = isZhengShu(fi, fo, wordAnalysis(fi, fo));
		while (sym == 29) {
			judge(sym, 29, fo);
			sym = wordAnalysis(fi, fo);
			judge(sym, 0, fo);
			sym = wordAnalysis(fi, fo);
			judge(sym, 27, fo);
			sym = isZhengShu(fi, fo, wordAnalysis(fi, fo));
		}
	}
	else if (sym == 6) {
		judge(sym, 6, fo);
		sym = wordAnalysis(fi, fo);
		judge(sym, 0, fo);
		sym = wordAnalysis(fi, fo);
		judge(sym, 27, fo);
		sym = wordAnalysis(fi, fo);
		judge(sym, 2, fo);
		sym = wordAnalysis(fi, fo);
		while (sym == 29) {
			judge(sym, 29, fo);
			sym = wordAnalysis(fi, fo);
			judge(sym, 0, fo);
			sym = wordAnalysis(fi, fo);
			judge(sym, 27, fo);
			sym = wordAnalysis(fi, fo);
			judge(sym, 2, fo);
			sym = wordAnalysis(fi, fo);
		}
	}
	else {
		error(5006);
	}
	fo << "<常量定义>" << endl;
	return sym;
}

int isWuFuHaoZhengShu(FILE* fi, ofstream& fo, int sym, int more_than_0) {
	cout << "wufuhaozhengshu: " << sym << endl;
	sym = wordAnalysis(fi, fo);
	if (more_than_0 == 1 && q.front().substr(7, 1) == "0") {				//	要求大于0
		error(1);
	}
	judge(sym, 1, fo);
	fo << "<无符号整数>" << endl;
	return wordAnalysis(fi, fo);
}

int isZhengShu(FILE* fi, ofstream& fo, int sym) {
	cout << "zhengshu: " << sym << endl;
	sym = wordAnalysis(fi, fo);
	if (sym == 17 || sym == 18) {
		judge(sym, sym, fo);
	}
	sym = isWuFuHaoZhengShu(fi, fo, sym);
	fo << "<整数>" << endl;
	return sym;
}

int isShengMingTouBu(FILE* fi, ofstream& fo, int sym) {
	sym = isLeiXingBiaoShiFu(fi, fo, sym);
	judge(sym, 0, fo);
	fo << "<声明头部>" << endl;
	return wordAnalysis(fi, fo);
}

int isBianLiangShuoMing(FILE* fi, ofstream& fo, int sym) {
	sym = isBianLiangDingYi(fi, fo, wordAnalysis(fi, fo));
	judge(sym, 28, fo);
	sym = wordAnalysis(fi, fo);
	while ((sym == 5 || sym == 6) && (wordAnalysis(fi, fo, 1) == 0) && ((sym = wordAnalysis(fi, fo, 1)) == 28 || sym == 29 || sym == 32)) {
		sym = isBianLiangDingYi(fi, fo, wordAnalysis(fi, fo));
		judge(sym, 28, fo);
		sym = wordAnalysis(fi, fo);
	}
	fo << "<变量说明>" << endl;
	return wordAnalysis(fi, fo);
}

int isBianLiangDingYi(FILE* fi, ofstream& fo, int sym) {
	sym = wordAnalysis(fi, fo);
	judge(sym, sym, fo);
	sym = wordAnalysis(fi, fo);
	judge(sym, 0, fo);
	sym = wordAnalysis(fi, fo);
	if (sym == 32) {
		judge(sym, 32, fo);
		sym = isWuFuHaoZhengShu(fi, fo, wordAnalysis(fi, fo), 1);
		judge(sym, 33, fo);
		sym = wordAnalysis(fi, fo);
	}
	while (sym == 29) {
		judge(sym, 29, fo);
		sym = wordAnalysis(fi, fo); 
		judge(sym, 0, fo);
		sym	= wordAnalysis(fi, fo);
		if (sym == 32) {
			judge(sym, 32, fo);
			sym = isWuFuHaoZhengShu(fi, fo, wordAnalysis(fi, fo), 1);
			judge(sym, 33, fo);
			sym = wordAnalysis(fi, fo);
		}
	}
	fo << "<变量定义>" << endl;
	return sym;
}

int isLeiXingBiaoShiFu(FILE* fi, ofstream& fo, int sym) {
	if (sym == 5 || sym == 6) {
		judge(sym, sym, fo);
	}
	else {
		error(5006);
	}
	return wordAnalysis(fi, fo);
}

int isYouHanShuDingYi(FILE* fi, ofstream& fo, int sym) {
	sym = isShengMingTouBu(fi, fo, sym);
	judge(sym, 30, fo);
	sym = isCanShuBiao(fi, fo, wordAnalysis(fi, fo));
	judge(sym, 31, fo);
	sym = wordAnalysis(fi, fo);
	judge(sym, 34, fo);
	sym = isFuHeYuJu(fi, fo, wordAnalysis(fi, fo));
	judge(sym, 35, fo);
	fo << "<有返回值函数定义>" << endl;
	return wordAnalysis(fi, fo);
}

int isVoidHanShuDingYi(FILE* fi, ofstream& fo, int sym) {
	judge(sym, 7, fo);
	sym = wordAnalysis(fi, fo);
	string name = q.front().substr(7, q.front().length() - 7);
	names.insert(name);
	judge(sym, 0, fo);
	sym = wordAnalysis(fi, fo);
	judge(sym, 30, fo);
	sym = isCanShuBiao(fi, fo, wordAnalysis(fi, fo));
	judge(sym, 31, fo);
	sym = wordAnalysis(fi, fo);
	judge(sym, 34, fo); 
	sym = isFuHeYuJu(fi, fo, wordAnalysis(fi, fo));
	judge(sym, 35, fo);
	fo << "<无返回值函数定义>" << endl; 
	return wordAnalysis(fi, fo);
}

int isFuHeYuJu(FILE* fi, ofstream& fo, int sym) {
	if (sym == 4) {
		sym = isChangLiangShuoMing(fi, fo, sym);
	}
	if (sym == 5 || sym == 6) {
		sym = isBianLiangShuoMing(fi, fo, sym);
	}
	sym = isYuJuLie(fi, fo, sym);
	fo << "<复合语句>" << endl;
	return sym;
}

int isCanShuBiao(FILE* fi, ofstream& fo, int sym) {
	if (sym == 5 || sym == 6) {
		sym = isLeiXingBiaoShiFu(fi, fo, sym);
		judge(sym, 0, fo);
		sym = wordAnalysis(fi, fo);
		while (sym == 29) {
			judge(sym, 29, fo);
			sym = isLeiXingBiaoShiFu(fi, fo, wordAnalysis(fi, fo));
			judge(sym, 0, fo);
			sym = wordAnalysis(fi, fo);
		}
	}
	fo << "<参数表>" << endl;
	return sym;
}

int isZhuHanShu(FILE* fi, ofstream& fo, int sym) {
	sym = wordAnalysis(fi, fo);
	judge(sym, 7, fo);
	sym = wordAnalysis(fi, fo);
	judge(sym, 8, fo);
	sym = wordAnalysis(fi, fo);
	judge(sym, 30, fo);
	sym = wordAnalysis(fi, fo);
	judge(sym, 31, fo);
	sym = wordAnalysis(fi, fo);
	judge(sym, 34, fo);
	sym = isFuHeYuJu(fi, fo, wordAnalysis(fi, fo));
	judge(sym, 35, fo);
	fo << "<主函数>" << endl;
	return sym;
}

int isBiaoDaShi(FILE* fi, ofstream& fo, int sym) {
	sym = wordAnalysis(fi, fo);
	//cout << "表达式: " << sym << endl;
	if (sym == 17 || sym == 18) {
		judge(sym, sym, fo);
	}
	sym = isXiang(fi, fo, wordAnalysis(fi, fo));
	while (sym == 17 || sym == 18) {
		judge(sym, sym, fo);
		sym = isXiang(fi, fo, wordAnalysis(fi, fo));
	}
	fo << "<表达式>" << endl;
	return sym;
}

int isXiang(FILE* fi, ofstream& fo, int sym) {
	sym = isYinZi(fi, fo, sym);
	while (sym == 19 || sym == 20) {
		judge(sym, sym, fo);
		sym = isYinZi(fi, fo, wordAnalysis(fi, fo));
	}
	fo << "<项>" << endl;
	return sym;
}

int isYinZi(FILE* fi, ofstream& fo, int sym) {
	if (sym == 0) {
		sym = wordAnalysis(fi, fo, 1);
		if (sym == 32) {
			sym = wordAnalysis(fi, fo);
			judge(sym, 0, fo);
			sym = wordAnalysis(fi, fo);
			judge(sym, 32, fo);
			sym = isBiaoDaShi(fi, fo, sym);
			judge(sym, 33, fo);
		}
		else if (sym == 30) {
			sym = isHanShuDiaoYongYuJu(fi, fo, sym);
		}
		else {
			sym = wordAnalysis(fi, fo);
			judge(sym, 0, fo);
		}
	}
	else if (sym == 30) {
		judge(sym, 30, fo);
		sym = isBiaoDaShi(fi, fo, wordAnalysis(fi, fo));
		judge(sym, 31, fo);
	} 
	else if (sym == 1 || sym == 17 || sym == 18) {
		sym = isZhengShu(fi, fo, sym);
	}
	else if (sym == 2) {
		judge(sym, 2, fo);
	}
	else {
		error(sym);
	}
	fo << "<因子>" << endl;
	return wordAnalysis(fi, fo);
}

int isYuJu(FILE* fi, ofstream& fo, int sym) {
	//cout << "语句: " << sym << endl;
	if (sym == 9) {
		sym = isTiaoJianYuJu(fi, fo, sym);
	}
	else if (11 <= sym && sym <= 13) {
		sym = isXunHuanYuJu(fi, fo, sym);
	}
	else if (sym == 34) {
		judge(sym, 34, fo);
		sym = isYuJuLie(fi, fo, wordAnalysis(fi, fo));
		judge(sym, 35, fo);
		sym = wordAnalysis(fi, fo);
	}
	else if (sym == 0) {
		if (wordAnalysis(fi, fo, 1) == 27 || LIST.back() == 32) {
			// LIST = {0, 27\32}
			sym = isFuZhiYuJu(fi, fo, sym);
		}
		else {
			// LIST = {0, 30}
			sym = isHanShuDiaoYongYuJu(fi, fo, sym);  // 合并了有返回值,和无返回值.
		}
		judge(sym, 28, fo);
	}
	else if (sym == 14) {
		sym = isDuYuJu(fi, fo, sym);
		judge(sym, 28, fo);
	}
	else if (sym == 15) {
		sym = isXieYuJu(fi, fo, sym);
		judge(sym, 28, fo);
	}
	else if (sym == 16) {
		sym = isFanHuiYuJu(fi, fo, sym);
		judge(sym, 28, fo);
	}
	else if (sym == 28) {
		judge(sym, 28, fo);
	}
	else {
		error(13141516);
	}
	fo << "<语句>" << endl;
	return wordAnalysis(fi, fo);
}

int isFuZhiYuJu(FILE* fi, ofstream& fo, int sym) {
	sym = wordAnalysis(fi, fo);
	judge(sym, 0, fo);
	sym = wordAnalysis(fi, fo);
	//cout << "赋值语句 " << sym << endl;
	if (sym == 32) {
		judge(sym, 32, fo);
		sym = isBiaoDaShi(fi, fo, wordAnalysis(fi, fo));
		judge(sym, 33, fo);
		sym = wordAnalysis(fi, fo);
	}
	judge(sym, 27, fo);	
	sym = isBiaoDaShi(fi, fo, wordAnalysis(fi, fo));
	fo << "<赋值语句>" << endl;
	return sym;
}

int isTiaoJianYuJu(FILE* fi, ofstream& fo, int sym) {
	sym = wordAnalysis(fi, fo);
	judge(sym, 9, fo);
	sym = wordAnalysis(fi, fo);
	judge(sym, 30, fo);
	sym = isTiaoJian(fi, fo, wordAnalysis(fi, fo));
	judge(sym, 31, fo);
	sym = isYuJu(fi, fo, wordAnalysis(fi, fo));
	if (sym == 10) {
		judge(sym, 10, fo);
		sym = isYuJu(fi, fo, wordAnalysis(fi, fo));
	}
	fo << "<条件语句>" << endl;
	return sym;
}

int isTiaoJian(FILE* fi, ofstream& fo, int sym) {
	sym = isBiaoDaShi(fi, fo, sym);				// 没有判断是否结果为整型
	if (21 <= sym && sym <= 26) {
		judge(sym, sym, fo);
		sym = isBiaoDaShi(fi, fo, wordAnalysis(fi, fo));
	}
	fo << "<条件>" << endl;
	return sym;
}

int isXunHuanYuJu(FILE* fi, ofstream& fo, int sym) {
	sym = wordAnalysis(fi, fo);
	if (sym == 11) {
		judge(sym, 11, fo);
		sym = isYuJu(fi, fo, wordAnalysis(fi, fo));
		judge(sym, 12, fo);
		sym = wordAnalysis(fi, fo);
		judge(sym, 30, fo);
		sym = isTiaoJian(fi, fo, wordAnalysis(fi, fo));
		judge(sym, 31, fo);
	}
	else if (sym == 12) {
		judge(sym, 12, fo);
		sym = wordAnalysis(fi, fo);
		judge(sym, 30, fo);
		sym = isTiaoJian(fi, fo, wordAnalysis(fi, fo));
		judge(sym, 31, fo);
		sym = isYuJu(fi, fo, wordAnalysis(fi, fo));
	}
	else if (sym == 13) {
		judge(sym, 13, fo);
		sym = wordAnalysis(fi, fo);
		judge(sym, 30, fo);
		sym = wordAnalysis(fi, fo);
		judge(sym, 0, fo);
		sym = wordAnalysis(fi, fo);
		judge(sym, 27, fo);
		//cout << "for XuanHuan: " << sym << endl;
		sym = isBiaoDaShi(fi, fo, wordAnalysis(fi, fo));
		judge(sym, 28, fo);
		sym = isTiaoJian(fi, fo, wordAnalysis(fi, fo));
		judge(sym, 28, fo);
		sym = wordAnalysis(fi, fo);
		judge(sym, 0, fo);
		sym = wordAnalysis(fi, fo);
		judge(sym, 27, fo);
		sym = wordAnalysis(fi, fo);
		judge(sym, 0, fo);
		sym = wordAnalysis(fi, fo);
		if (sym == 17 || sym == 18) {
			judge(sym, sym, fo);
		}
		else {
			error(170018);
		}
		sym = isBuChang(fi, fo, wordAnalysis(fi, fo));
		judge(sym, 31, fo);
		sym = isYuJu(fi, fo, wordAnalysis(fi, fo));
	}
	fo << "<循环语句>" << endl;
	return wordAnalysis(fi, fo);
}

int isBuChang(FILE* fi, ofstream& fo, int sym) {
	sym = isWuFuHaoZhengShu(fi, fo, sym);
	fo << "<步长>" << endl;
	return sym;
}

int isHanShuDiaoYongYuJu(FILE* fi, ofstream& fo, int sym) {
	sym = wordAnalysis(fi, fo);
	string name = q.front().substr(7, q.front().length() - 7);
	judge(sym, 0, fo);
	sym = wordAnalysis(fi, fo);
	judge(sym, 30, fo);
	sym = isZhiCanShuBiao(fi, fo, wordAnalysis(fi, fo));
	judge(sym, 31, fo);
	if (names.count(name) == 1) {
		fo << "<无返回值函数调用语句>" << endl;
	}
	else {
		fo << "<有返回值函数调用语句>" << endl;
	}
	return wordAnalysis(fi, fo);
}

int isZhiCanShuBiao(FILE* fi, ofstream& fo, int sym) {
	if ((0 <= sym && sym <= 2) || sym == 17 || sym == 18 || sym == 30) {
		sym = isBiaoDaShi(fi, fo, sym);
		while (sym == 29) {
			judge(sym, 29, fo);
			sym = isBiaoDaShi(fi, fo, wordAnalysis(fi, fo));
		}
	}
	fo << "<值参数表>" << endl;
	return wordAnalysis(fi, fo);
}

int isYuJuLie(FILE* fi, ofstream& fo, int sym) {
	while (sym == 0 || sym == 9 || (11 <= sym && sym <= 16) || (sym == 28) || (sym == 34)) {
		sym = isYuJu(fi, fo, sym);
	}
	fo << "<语句列>" << endl;
	return sym;
}

int isDuYuJu(FILE* fi, ofstream& fo, int sym) {
	sym = wordAnalysis(fi, fo);
	judge(sym, 14, fo);
	sym = wordAnalysis(fi, fo);
	judge(sym, 30, fo);
	sym = wordAnalysis(fi, fo);
	judge(sym, 0, fo);
	sym = wordAnalysis(fi, fo);
	while (sym == 29) {
		judge(sym, 29, fo);
		sym = wordAnalysis(fi, fo);
		judge(sym, 0, fo);
		sym = wordAnalysis(fi, fo);
	}
	judge(sym, 31, fo);
	fo << "<读语句>" << endl;
	return wordAnalysis(fi, fo);
}

int isXieYuJu(FILE* fi, ofstream& fo, int sym) {
	sym = wordAnalysis(fi, fo);
	judge(sym, 15, fo);
	sym = wordAnalysis(fi, fo);
	judge(sym, 30, fo);
	sym = wordAnalysis(fi, fo);
	if (sym == 3) {
		judge(sym, 3, fo);
		sym = wordAnalysis(fi, fo);
		if (sym == 29) {
			judge(sym, 29, fo);
			sym = isBiaoDaShi(fi, fo, wordAnalysis(fi, fo));
		}
	}
	else {
		sym = isBiaoDaShi(fi, fo, sym);
	}
	judge(sym, 31, fo);
	fo << "<写语句>" << endl;
	return wordAnalysis(fi, fo);
}

int isFanHuiYuJu(FILE* fi, ofstream& fo, int sym) {
	sym = wordAnalysis(fi, fo);
	judge(sym, 16, fo);
	sym = wordAnalysis(fi, fo);
	if (sym == 30) {
		judge(sym, 30, fo);
		sym = isBiaoDaShi(fi, fo, wordAnalysis(fi, fo));
		judge(sym, 31, fo);
	}
	fo << "<返回语句>" << endl;
	return wordAnalysis(fi, fo);
}

void grammarAnalysis(FILE* fi, ofstream &fo) {
	int sym = -1;;
	sym = wordAnalysis(fi, fo);
	if (sym == 4) {
		sym = isChangLiangShuoMing(fi, fo, sym);
	}
	if (sym == 5 || sym == 6) {			// 区分 有返回值函数定义  变量说明
		// LIST = {5\6}
		sym = wordAnalysis(fi, fo, 1);
		// LIST = {5\6, 0}
		sym = wordAnalysis(fi, fo, 1);
		if (sym != 30) {
			// LIST = {5\6, 0, 28\29\32}
			sym = isBianLiangShuoMing(fi, fo, sym);
		}
		else {
			sym = LIST.front();
		}
	}
	while (sym == 5 || sym == 6 || sym == 7) {
		if (sym == 5 || sym == 6) {
			// LIST = {5\6, 0, 30}
			// LIST = {5\6}
			sym = isYouHanShuDingYi(fi, fo, sym);
		}
		else {
			if (wordAnalysis(fi, fo, 1) == 8) {
				break;
			}
			// LIST = {7, 0}
			sym = isVoidHanShuDingYi(fi, fo, sym);
		}
	}
	// LIST = {7, 8}
	sym = isZhuHanShu(fi, fo, sym);
	fo << "<程序>" << endl;
	return;
}
