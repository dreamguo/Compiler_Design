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

const int DEBUG_ERROR = 1;

const int DEBUG_MIDDLE = 0;
const int DEBUG_MIPS = 0;
const int DEBUG_INLINE = 0;
const int DEBUG_FOR = 0;

const string NULL_CHAR = "!";

extern list<int> LIST;
extern queue<string> q;
extern map<string, list<int>> global_labels;
extern map<string, list<int>> local_labels;
extern int LINE_NUM;
extern int LINE_NUM_LAST;
extern int isLocalFunc;
extern int IS_ANY_ERROR;
extern FILE* fi;

class Siyuanshi {
public:
	string op;
	string result;
	string result_type;
	string leftnum;
	string left_type;
	string rightnum;
	string right_type;

	Siyuanshi(string op_, string result_, string leftnum_, string rightnum_,
		string result_type_ = "0", string left_type_ = "0", string right_type_ = "0");
	string output_string();
};

extern list<Siyuanshi> siyuanshi_list;
extern list<Siyuanshi> siyuanshi_data_list;

void tofo(string str);
void tofe(string str);
void tofmips(string str);
void tomiddle(string str);
void error(int a, char type = '-', int line_num = -1);

int wordAnalysis(int force = 0);
void grammarAnalysis();
void judge(int sym, int a, char type = '-', int fuhao = 0);
list<int> find_label(string name = "", int print = 1);
void diaoyong_label(string name, int fuhao, int type);
string add_op(string op, string label_input = "");
void add_iden(int type, int iden_type = 0);
void generate_mips();
void optimal();

// Ŀǰ��mips����ÿ������ĩβ����add_op(ret),��ֹû��return���ĺ���.

// ����   ͳһʹ�� $gpջ
// ����ֵ ͳһʹ�� $v1

// judge������
// fuhao: 0=Ĭ��, 1=const int, 2=const char, 3=int, 4=char, 
//		  5=void func, 6=int func, 7=char func, 8=end of canshubiao, 12=const(int\char)

// add_op������
// ��һ��[ 1]��ʾ�Ƿ�Ϊ ����, �ڶ���[01]��ʾ�Ƿ�Ϊ ȫ����
// type: 0=Ĭ��, 1 [01]=const int, 2 [01]=const char, 3[ 1][01]=int, 4[ 1][01]=char,
// [ 1]��ʾ�Ƿ�Ϊ char����ʱ����
//		 5=��int, 6=��char, 7=��str, 8[ 1]=�Ĵ���(�����int), 9=int return, 10=char return				

/*
�Ƿ����Ż򲻷��ϴʷ�										a OK
�����ض���													b OK
δ���������												c OK
��������������ƥ��											d OK
�����������Ͳ�ƥ��											e OK
�����ж��г��ֲ��Ϸ�������									f OK
�޷���ֵ�ĺ������ڲ�ƥ���return���						g OK
�з���ֵ�ĺ���ȱ��return������ڲ�ƥ���return���		h OK
����Ԫ�ص��±�ֻ�������ͱ��ʽ								i OK
���ܸı䳣����ֵ											j OK ������forѭ������������"��ֵ���"
ӦΪ�ֺ�													k OK û����xxx;xxx;���˵�һ�����ŵ����
ӦΪ��С����')��											l OK
ӦΪ��������']'												m OK
do-whileӦΪ�����ȱ��while									n OK
����������=����ֻ�������ͻ��ַ��ͳ���						o OK
*/

/*
	-2  �Ƿ��ַ�
	-1	��
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

/*
inline:
	op == "hanshu"
	op == "+" || op == "-" || op == "*" || op == "/"
	op == "--" || op == "="
	op == "print" || op == "print2" || op == "scanf"
	op == "para"
	op == "ret()" || op == "ret"
	
not inline:
	op == "push" || op == "call_end" || op == "call_start"
	op == "set_label" || op == "jump" || op == "BZ_jump" || op == "BNZ_jump"
	op == "var" || op == "var[]" 
	op == "[]store" || op == "[]load"
	op == "==" || op == "!=" || op == ">=" || op == ">" || op == "<=" || op == "<"
*/