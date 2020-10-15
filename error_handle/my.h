#pragma once
#include <set> 
#include <map>
#include <list>
#include <queue>
#include <stack>
#include <string>
#include <fstream>
#include <iterator>
#include <assert.h>
#include <iostream>
using namespace std;

extern list<int> LIST;
extern queue<string> q;
extern set<string> names;
extern map<string, list<int>> global_labels;
extern map<string, list<int>> local_labels;
extern int LINE_NUM;
extern int LINE_NUM_LAST;
extern int isLocalFunc;
extern FILE* fi;
extern ofstream fo;
extern ofstream fe;

void tofe(string str);
void tofo(string str);
void error(int a, char type = '-', int line_num = -1);

int wordAnalysis(int force = 0);
void grammarAnalysis();
void judge(int sym, int a, char type = '-', int fuhao = 0);
list<int> find_label(string name = "", int print = 1);
void diaoyong_label(string name, int fuhao, int type);

/*		
非法符号或不符合词法										a OK
名字重定义													b OK
未定义的名字												c OK
函数参数个数不匹配											d OK
函数参数类型不匹配											e OK
条件判断中出现不合法的类型									f OK
无返回值的函数存在不匹配的return语句						g OK
有返回值的函数缺少return语句或存在不匹配的return语句		h OK
数组元素的下标只能是整型表达式								i OK
不能改变常量的值											j OK 考虑了for循环语句条件里的"赋值语句"
应为分号													k OK 没考虑xxx;xxx;少了第一个逗号的情况
应为右小括号')’											l OK
应为右中括号']'												m OK
do-while应为语句中缺少while									n OK
常量定义中=后面只能是整型或字符型常量						o OK
*/

/*
	-2  非法字符
	-1	无
	0	idenFR
	1	intCON
	2	charCON
	3	strCON
	4	const
	5	int
	6	char
	7	void
	8	main
	9	if
	10	else
	11	do
	12	while
	13	for
	14	sacnf
	15	printf
	16	return
	17	+
	18	-
	19	*
	20	/
	21	<
	22	<=
	23	>
	24	>=
	25	==
	26	!=
	27	=
	28	;
	29	,
	30	(
	31	)
	32	[
	33	]
	34	{
	35	}
	100	!
*/
