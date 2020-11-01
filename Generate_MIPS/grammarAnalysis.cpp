#include "my.h"

int isZhiFuChuan(int sym);
int isChangLiangShuoMing(int sym);
int isChangLiangDingYi(int sym);
int isWuFuHaoZhengShu(int sym, int more_than_0 = 0, char wrong_type = '-');
int isZhengShu(int sym, char wrong_type = '-');
int isShengMingTouBu(int sym);
int isBianLiangShuoMing(int sym);
int isBianLiangDingYi(int sym);
int isYouHanShuDingYi(int sym);
int isVoidHanShuDingYi(int sym);
int isFuHeYuJu(int sym);
int isCanShuBiao(int sym);
int isZhuHanShu(int sym);
int isBiaoDaShi(int sym);
int isXiang(int sym);
int isYinZi(int sym);
int isYuJu(int sym);
int isFuZhiYuJu(int sym);
int isTiaoJianYuJu(int sym);
int isTiaoJian(int sym);
int isXunHuanYuJu(int sym);
int isBuChang(int sym);
int isHanShuDiaoYongYuJu(int sym, int ret);
int isZhiCanShuBiao(int sym);
int isYuJuLie(int sym);
int isDuYuJu(int sym);
int isXieYuJu(int sym);
int isFanHuiYuJu(int sym);

int tmpLocalFunc = 0;
int biaodashi_type = 0; // 1=int, 2=char
int has_return = 0;

int isZhiFuChuan(int sym) {
	sym = wordAnalysis();
	add_iden(3);
	judge(sym, 3);
	tofo("<字符串>");
	return wordAnalysis();
}

int isChangLiangShuoMing(int sym) {
	while (sym == 4) {
		judge(sym, 4);
		sym = isChangLiangDingYi(wordAnalysis());
		judge(sym, 28);
		sym = wordAnalysis();
	}
	tofo("<常量说明>");
	return sym;
}

int isChangLiangDingYi(int sym) {
	if (sym == 5) {
		judge(sym, 5);
		sym = wordAnalysis();
		// add_iden(0, 1);
		judge(sym, 0, 'b', 1);
		sym = wordAnalysis();
		judge(sym, 27);
		sym = isZhengShu(wordAnalysis(), 'o');

		string str = "=";
		add_op(str);
		while (sym == 29) {
			judge(sym, 29);
			sym = wordAnalysis();
			// add_iden(0, 1);
			judge(sym, 0, 'b', 1);
			sym = wordAnalysis();
			judge(sym, 27);
			sym = isZhengShu(wordAnalysis(), 'o');
			add_op("=");
		}
	}
	else if (sym == 6) {
		judge(sym, 6);
		sym = wordAnalysis();
		// add_iden(0, 2);
		judge(sym, 0, 'b', 2);
		sym = wordAnalysis();
		judge(sym, 27);
		sym = wordAnalysis();
		add_iden(2);
		add_op("="); 
		judge(sym, 2, 'o');
		sym = wordAnalysis();
		while (sym == 29) {
			judge(sym, 29);
			sym = wordAnalysis();
			// add_iden(0, 2);
			judge(sym, 0, 'b', 2);
			sym = wordAnalysis();
			judge(sym, 27);
			sym = wordAnalysis();
			add_iden(2);
			add_op("=");
			judge(sym, 2, 'o');
			sym = wordAnalysis();
		}
	}
	else {
		error(5006);
	}
	tofo("<常量定义>");
	return sym;
}

int isWuFuHaoZhengShu(int sym, int more_than_0, char wrong_type) {
	sym = wordAnalysis();
	if (more_than_0 == 1 && q.front().size() > 7 && q.front().substr(7, 1) == "0") {				//	要求大于0
		error(1);
	}
	add_iden(1);
	judge(sym, 1, wrong_type);
	tofo("<无符号整数>");
	return wordAnalysis();
}

int isZhengShu(int sym, char wrong_type) {
	sym = wordAnalysis();
	int tmp = 0;
	if (sym == 17 || sym == 18) {
		tmp = sym;
		judge(sym, sym);
	}
	sym = isWuFuHaoZhengShu(wordAnalysis(), 0, wrong_type);
	if (tmp == 18) {
		add_op("--");
	}
	tofo("<整数>");
	return sym;
}

int isShengMingTouBu(int sym) {
	int fuhao = 0;
	sym = wordAnalysis();
	if (sym == 5 || sym == 6) {
		judge(sym, sym);
		tmpLocalFunc = sym - 3;
		fuhao = sym + 1;		// 危险
	}
	else {
		error(5006);
	}
	sym = wordAnalysis();
	judge(sym, 0, 'b', fuhao);
	tofo("<声明头部>");
	return wordAnalysis();
}

int isBianLiangShuoMing(int sym) {
	// LIST = {5\6, 0, 28\29\32}
	sym = isBianLiangDingYi(wordAnalysis());
	judge(sym, 28);
	sym = wordAnalysis();
	while ((sym == 5 || sym == 6) && (wordAnalysis(1) == 0) && ((sym = wordAnalysis(1)) == 28 || sym == 29 || sym == 32)) {
		sym = isBianLiangDingYi(wordAnalysis());
		judge(sym, 28);
		sym = wordAnalysis();
	}
	tofo("<变量说明>");
	return wordAnalysis();
}

int isBianLiangDingYi(int sym) {
	int fuhao;
	sym = wordAnalysis();
	judge(sym, sym);
	if (sym == 5) {
		fuhao = 3;
	}
	else {
		fuhao = 4;
	}
	sym = wordAnalysis();
	// 变量不需要add_iden, 已经通过judge函数申请了空间和记录了名字
	judge(sym, 0, 'b', fuhao);
	sym = wordAnalysis();
	if (sym == 32) {
		judge(sym, 32);
		sym = isWuFuHaoZhengShu(wordAnalysis(), 1, 'i');
		judge(sym, 33);
		add_op("[]init"); 
		sym = wordAnalysis();
	}
	while (sym == 29) {
		judge(sym, 29);
		sym = wordAnalysis();
		judge(sym, 0, 'b', fuhao);
		sym = wordAnalysis();
		if (sym == 32) {
			judge(sym, 32);
			sym = isWuFuHaoZhengShu(wordAnalysis(), 1, 'i');
			judge(sym, 33);
			add_op("[]init");
			sym = wordAnalysis();
		}
	}
	tofo("<变量定义>");
	return sym;
}


int isYouHanShuDingYi(int sym) {
	sym = isShengMingTouBu(sym);
	judge(sym, 30);
	isLocalFunc = tmpLocalFunc;
	sym = isCanShuBiao(wordAnalysis());
	isLocalFunc = 0;
	judge(0, 0, 'b', 8);	// 表示参数表完成,可以进栈(全局栈).  只是推进栈,不会有任何其他操作.
	isLocalFunc = tmpLocalFunc;
	judge(sym, 31);
	sym = wordAnalysis();
	judge(sym, 34);
	sym = isFuHeYuJu(wordAnalysis());
	judge(sym, 35);
	isLocalFunc = 0;
	local_labels.clear();
	tofo("<有返回值函数定义>");
	return wordAnalysis();
}

int isVoidHanShuDingYi(int sym) {
	judge(sym, 7);
	sym = wordAnalysis();
	judge(sym, 0, 'b', 5);
	sym = wordAnalysis();
	judge(sym, 30);
	isLocalFunc = 1;
	sym = isCanShuBiao(wordAnalysis());
	isLocalFunc = 0;
	judge(0, 0, 'b', 8);	// 表示参数表完成,可以进栈.
	isLocalFunc = 1;
	judge(sym, 31);
	sym = wordAnalysis();
	judge(sym, 34);
	sym = isFuHeYuJu(wordAnalysis());
	judge(sym, 35);
	isLocalFunc = 0;
	local_labels.clear();
	add_op("ret"); 
	tofo("<无返回值函数定义>");
	return wordAnalysis();
}

int isFuHeYuJu(int sym) {
	if (sym == 4) {
		sym = isChangLiangShuoMing(sym);
	}
	if (sym == 5 || sym == 6) {
		sym = isBianLiangShuoMing(sym);
	}
	has_return = 0;
	sym = isYuJuLie(sym);
	if (has_return == 0 && isLocalFunc >= 2) {
		error(0, 'h');
	}
	tofo("<复合语句>");
	return sym;
}

int isCanShuBiao(int sym) {
	int fuhao;
	if (sym == 5 || sym == 6) {
		fuhao = sym - 2;		// TODO: 很危险
		judge(sym, sym);
		sym = wordAnalysis();
		judge(sym, 0, 'b', fuhao);
		sym = wordAnalysis();
		while (sym == 29) {
			judge(sym, 29);
			sym = wordAnalysis();
			if (sym == 5 || sym == 6) {
				judge(sym, sym);
				fuhao = sym - 2;
			}
			else {
				error(5006);
			}
			sym = wordAnalysis();
			judge(sym, 0, 'b', fuhao);
			sym = wordAnalysis();
		}
	}
	tofo("<参数表>");
	return sym;
}

int isZhuHanShu(int sym) {
	sym = wordAnalysis();
	judge(sym, 7);
	sym = wordAnalysis();
	judge(sym, 8);
	Siyuanshi siyuanshi("hanshu", "main", NULL_CHAR, NULL_CHAR, "5", "0", "0");
	siyuanshi_list.push_front(siyuanshi);
	sym = wordAnalysis();
	judge(sym, 30);
	sym = wordAnalysis();
	isLocalFunc = 1;
	judge(sym, 31);
	sym = wordAnalysis();
	judge(sym, 34);
	sym = isFuHeYuJu(wordAnalysis());
	judge(sym, 35);
	isLocalFunc = 0;
	add_op("ret");
	tofo("<主函数>");
	return sym;
}

int isBiaoDaShi(int sym) {
	sym = wordAnalysis();
	int tmp = 0;
	if (sym == 17 || sym == 18) {
		biaodashi_type = 1;
		tmp = sym;
		judge(sym, sym);
	}
	sym = isXiang(wordAnalysis());
	if (tmp == 18) {
		add_op("--");
	}
	while (sym == 17 || sym == 18) {
		biaodashi_type = 1;
		tmp = sym;
		judge(sym, sym);
		sym = isXiang(wordAnalysis());
		if (tmp == 18) {
			add_op("-");
		}
		else if (tmp == 17) {
			add_op("+");
		}
	}
	tofo("<表达式>");
	return sym;
}

int isXiang(int sym) {
	sym = isYinZi(sym);
	int tmp = 0;
	while (sym == 19 || sym == 20) {
		biaodashi_type = 1;
		tmp = sym;
		judge(sym, sym);
		sym = isYinZi(wordAnalysis());
		if (tmp == 19) {
			add_op("*");
		}
		else if (tmp == 20) {
			add_op("/");
		}
	}
	tofo("<项>");
	return sym;
}

int isYinZi(int sym) {
	if (sym == 0) {
		list<int> tmp = find_label("", 0);
		if (tmp.front() == 1 || tmp.front() == 3 || tmp.front() == 6) {
			biaodashi_type = 1;
		}
		sym = wordAnalysis(1);
		if (sym == 32) {
			sym = wordAnalysis();
			add_iden(0);
			judge(sym, 0, 'c');
			sym = wordAnalysis();
			judge(sym, 32);
			int tmp = biaodashi_type;
			biaodashi_type = 2;
			sym = isBiaoDaShi(sym);
			if (biaodashi_type != 1) {
				error(-1, 'i');
			}
			biaodashi_type = tmp;
			judge(sym, 33);
			add_op("[]load");
		}
		else if (sym == 30) {
			sym = isHanShuDiaoYongYuJu(sym, 1);
		}
		else {
			sym = wordAnalysis();
			add_iden(0);
			judge(sym, 0, 'c');
		}
	}
	else if (sym == 30) {
		biaodashi_type = 1;
		judge(sym, 30);
		sym = isBiaoDaShi(wordAnalysis());
		judge(sym, 31);
		add_op("()");
	}
	else if (sym == 1 || sym == 17 || sym == 18) {
		biaodashi_type = 1;
		sym = isZhengShu(sym);
	}
	else if (sym == 2) {
		add_iden(2);
		judge(sym, 2);
	}
	else {
		error(sym);
	}
	tofo("<因子>");
	return wordAnalysis();
}

int isYuJu(int sym) {
	if (sym == 9) {
		sym = isTiaoJianYuJu(sym);
	}
	else if (11 <= sym && sym <= 13) {
		sym = isXunHuanYuJu(sym);
	}
	else if (sym == 34) {
		judge(sym, 34);
		sym = isYuJuLie(wordAnalysis());
		judge(sym, 35);
		sym = wordAnalysis();
	}
	else if (sym == 0) {
		if (wordAnalysis(1) == 27 || LIST.back() == 32) {
			// LIST = {0, 27\32}
			sym = isFuZhiYuJu(sym);
		}
		else {
			// LIST = {0, 30}
			sym = isHanShuDiaoYongYuJu(sym, 0);  // 合并了有返回值,和无返回值.
		}
		judge(sym, 28);
	}
	else if (sym == 14) {
		sym = isDuYuJu(sym);
		judge(sym, 28);
	}
	else if (sym == 15) {
		sym = isXieYuJu(sym);
		judge(sym, 28);
	}
	else if (sym == 16) {
		sym = isFanHuiYuJu(sym);
		judge(sym, 28);
	}
	else if (sym == 28) {
		judge(sym, 28);
	}
	else {
		error(13141516);
	}
	tofo("<语句>");
	return wordAnalysis();
}

int isFuZhiYuJu(int sym) {
	sym = wordAnalysis();
	add_iden(0);
	judge(sym, 0, 'c', 12);
	sym = wordAnalysis();
	if (sym == 32) {
		judge(sym, 32);
		biaodashi_type = 2; // 初始化为char,因为只要有一个int,这个表达式就是int
		sym = isBiaoDaShi(wordAnalysis());
		if (biaodashi_type != 1) {
			error(0, 'i');
		}
		judge(sym, 33);
		add_op("[]store");
		sym = wordAnalysis();
	}
	judge(sym, 27);
	biaodashi_type = 2; // 初始化为char,因为只要有一个int,这个表达式就是int
	sym = isBiaoDaShi(wordAnalysis());
	add_op("=");
	tofo("<赋值语句>");
	return sym;
}

int isTiaoJianYuJu(int sym) {
	sym = wordAnalysis();
	judge(sym, 9);
	sym = wordAnalysis();
	judge(sym, 30);
	sym = isTiaoJian(wordAnalysis());
	string else_label = add_op("BNZ_jump");
	judge(sym, 31);
	sym = isYuJu(wordAnalysis());
	if (sym == 10) {
		string end_label = add_op("jump");
		add_op("set_label", else_label);
		judge(sym, 10);
		sym = isYuJu(wordAnalysis());
		add_op("set_label", end_label);
	}
	else {
		add_op("set_label", else_label);
	}
	tofo("<条件语句>");
	return sym;
}

int isTiaoJian(int sym) {
	biaodashi_type = 2; // 初始化为char,因为只要有一个int,这个表达式就是int
	sym = isBiaoDaShi(sym);				// 没有判断是否结果为整型
	if (biaodashi_type != 1) {
		error(0, 'f');
	}
	if (21 <= sym && sym <= 26) {
		int tmp = sym;
		judge(sym, sym);
		biaodashi_type = 2; // 初始化为char,因为只要有一个int,这个表达式就是int
		sym = isBiaoDaShi(wordAnalysis());
		if (biaodashi_type != 1) {
			error(0, 'f');
		}
		switch (tmp) {
		case 21:
			add_op("<");
			break;
		case 22:
			add_op("<=");
			break;
		case 23:
			add_op(">");
			break;
		case 24:
			add_op(">=");
			break;
		case 25:
			add_op("==");
			break;
		case 26:
			add_op("!=");
			break;
		}
	}
	tofo("<条件>");
	return sym;
}

int isXunHuanYuJu(int sym) {
	sym = wordAnalysis();
	if (sym == 11) {
		string start_label = add_op("set_xunhuan_start_label");
		judge(sym, 11);
		sym = isYuJu(wordAnalysis());
		judge(sym, 12, 'n');
		sym = wordAnalysis();
		judge(sym, 30);
		sym = isTiaoJian(wordAnalysis());
		judge(sym, 31);
		add_op("BZ_jump", start_label);
	}
	else if (sym == 12) {
		string start_label = add_op("set_xunhuan_start_label");
		judge(sym, 12);
		sym = wordAnalysis();
		judge(sym, 30);
		sym = isTiaoJian(wordAnalysis());
		string end_label = add_op("BNZ_jump");
		judge(sym, 31);
		sym = isYuJu(wordAnalysis());
		add_op("jump", start_label);
		add_op("set_label", end_label);
	}
	else if (sym == 13) {
		judge(sym, 13);
		sym = wordAnalysis();
		judge(sym, 30);
		sym = wordAnalysis();
		add_iden(0);
		judge(sym, 0, 'c', 12);
		sym = wordAnalysis();
		judge(sym, 27);
		biaodashi_type = 2; // 初始化为char,因为只要有一个int,这个表达式就是int
		sym = isBiaoDaShi(wordAnalysis());
		judge(sym, 28, 'K');
		add_op("=");
		string start_label = add_op("set_xunhuan_start_label");
		sym = isTiaoJian(wordAnalysis());
		string end_label = add_op("BNZ_jump");
		judge(sym, 28, 'K');
		sym = wordAnalysis();
		add_iden(0); 
		judge(sym, 0, 'c');
		sym = wordAnalysis();
		judge(sym, 27);
		sym = wordAnalysis();
		add_iden(0); 
		judge(sym, 0, 'c', 12);
		sym = wordAnalysis();
		int tmp = sym;
		if (sym == 17 || sym == 18) {
			judge(sym, sym);
		}
		else {
			error(170018);
		}
		sym = isBuChang(wordAnalysis());
		judge(sym, 31);
		sym = isYuJu(wordAnalysis());
		if (tmp == 17) {
			add_op("+");
		}
		else if (tmp == 18){
			add_op("-");
		}
		add_op("=");
		add_op("jump", start_label); 
		add_op("set_label", end_label);
	}
	tofo("<循环语句>");
	return wordAnalysis();
}

int isBuChang(int sym) {
	sym = isWuFuHaoZhengShu(sym);
	tofo("<步长>");
	return sym;
}

int isHanShuDiaoYongYuJu(int sym, int ret) {
	sym = wordAnalysis();
	string name = q.front().substr(7, q.front().length() - 7);
	diaoyong_label(name, 0, 0);
	judge(sym, 0, 'c');
	sym = wordAnalysis();
	judge(sym, 30);
	sym = isZhiCanShuBiao(wordAnalysis());
	judge(sym, 31);
	diaoyong_label(name, 0, 1);
	if (find_label(name).front() == 5) {
		tofo("<无返回值函数调用语句>");
	}
	else if (find_label(name).front() == 6 || find_label(name).front() == 7) {
		tofo("<有返回值函数调用语句>");
		if (ret == 1) {
			add_iden(find_label(name).front() - 2);  // 4 / 5
		}
	}
	return wordAnalysis();
}

int isZhiCanShuBiao(int sym) {
	if ((0 <= sym && sym <= 2) || sym == 17 || sym == 18 || sym == 30) {
		int tmp = biaodashi_type;
		biaodashi_type = 2; // 初始化为char,因为只要有一个int,这个表达式就是int
		sym = isBiaoDaShi(sym);
		diaoyong_label("", biaodashi_type, 2);
		add_op("push");
		biaodashi_type = tmp;
		while (sym == 29) {
			judge(sym, 29);
			tmp = biaodashi_type;
			biaodashi_type = 2; // 初始化为char,因为只要有一个int,这个表达式就是int
			sym = isBiaoDaShi(wordAnalysis());
			diaoyong_label("", biaodashi_type, 2);
			add_op("push");
			biaodashi_type = tmp;
		}
	}
	tofo("<值参数表>");
	return wordAnalysis();
}

int isYuJuLie(int sym) {
	while (sym == 0 || sym == 9 || (11 <= sym && sym <= 16) || (sym == 28) || (sym == 34)) {
		sym = isYuJu(sym);
	}
	tofo("<语句列>");
	return sym;
}

int isDuYuJu(int sym) {
	sym = wordAnalysis();
	judge(sym, 14);
	sym = wordAnalysis();
	judge(sym, 30);
	sym = wordAnalysis();
	add_iden(0);
	judge(sym, 0, 'c');
	sym = wordAnalysis();
	add_op("scanf");
	while (sym == 29) {
		judge(sym, 29);
		sym = wordAnalysis();
		add_iden(0);
		judge(sym, 0, 'c');
		sym = wordAnalysis();
		add_op("scanf");
	}
	judge(sym, 31);
	tofo("<读语句>");
	return wordAnalysis();
}

int isXieYuJu(int sym) {
	sym = wordAnalysis();
	judge(sym, 15);
	sym = wordAnalysis();
	judge(sym, 30);
	sym = wordAnalysis();
	int tmp = 0;
	if (sym == 3) {
		sym = isZhiFuChuan(sym);
		if (sym == 29) {
			tmp = 1;
			judge(sym, 29);
			biaodashi_type = 2; // 初始化为char,因为只要有一个int,这个表达式就是int
			sym = isBiaoDaShi(wordAnalysis());
		}
	}
	else {
		biaodashi_type = 2; // 初始化为char,因为只要有一个int,这个表达式就是int
		sym = isBiaoDaShi(sym);
	}
	judge(sym, 31);
	if (tmp == 0) {
		add_op("print");
	} else {
		add_op("print2");
	}
	tofo("<写语句>");
	return wordAnalysis();
}

int isFanHuiYuJu(int sym) {
	has_return = 1;
	sym = wordAnalysis();
	judge(sym, 16);
	sym = wordAnalysis();
	int tmp = 0;
	if (sym == 30) {
		tmp = 1;
		if (isLocalFunc == 1) {
			error(30, 'g');
		}
		judge(sym, 30);
		biaodashi_type = 2; // 初始化为char,因为只要有一个int,这个表达式就是int
		sym = isBiaoDaShi(wordAnalysis());
		if (isLocalFunc != 1 && isLocalFunc != biaodashi_type + 1) {
			error(30, 'h');
		}
		judge(sym, 31);
	}
	else if (isLocalFunc != 1) {
		error(30, 'h');
	}
	if (tmp == 1) {
		add_op("ret()");
	}
	else {
		add_op("ret");
	}
	tofo("<返回语句>");
	return wordAnalysis();
}

void grammarAnalysis() {
	int sym = -1;;
	sym = wordAnalysis();
	if (sym == 4) {
		sym = isChangLiangShuoMing(sym);
	}
	if (sym == 5 || sym == 6) {			// 区分 有返回值函数定义  变量说明
		// LIST = {5\6}
		sym = wordAnalysis(1);
		// LIST = {5\6, 0}
		sym = wordAnalysis(1);
		if (sym != 30) {
			// LIST = {5\6, 0, 28\29\32}
			sym = isBianLiangShuoMing(sym);
		}
		else {
			sym = LIST.front();
		}
	}
	while (sym == 5 || sym == 6 || sym == 7) {
		if (sym == 5 || sym == 6) {
			// LIST = {5\6, 0, 30}
			// LIST = {5\6}
			sym = isYouHanShuDingYi(sym);
		}
		else {
			if (wordAnalysis(1) == 8) {
				break;
			}
			// LIST = {7, 0}
			sym = isVoidHanShuDingYi(sym);
		}
	}
	// LIST = {7, 8}
	sym = isZhuHanShu(sym);
	tofo("<程序>");
	return;
}
