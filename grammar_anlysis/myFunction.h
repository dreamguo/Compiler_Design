#pragma once
#include <set> 
#include <list>
#include <queue>
#include <string>
#include <fstream>
#include <iostream>
using namespace std;

extern list<int> LIST;
extern queue<string> q;
extern set<string> names;

int wordAnalysis(FILE* fi, ofstream &fo, int force=0);
void grammarAnalysis(FILE* fi, ofstream &fo);
void error(int a);

/*
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