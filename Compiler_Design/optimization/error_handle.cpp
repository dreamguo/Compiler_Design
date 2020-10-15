#include "my.h"

list<int> canshu_dingyi;
string funcName_dingyi;
list<int> canshu_diaoyong;
list<string> funcName_diaoyong;


// type: 0=函数名 1=调用尾部, 有嵌套调用
void diaoyong_label(string name, int fuhao, int type) {
	if (type == 0) {
		list<int> dingyi = find_label(name, 0);
		canshu_diaoyong.push_back(8);
		canshu_diaoyong.push_back(dingyi.front());
		Siyuanshi siyuanshi("call_start", name, NULL_CHAR, NULL_CHAR);
		siyuanshi_list.push_front(siyuanshi);
		funcName_diaoyong.push_front(name);
	}
	else if (type == 1) {
		// 比较的时候到了
		list<int> dingyi = find_label(funcName_diaoyong.front(), 0);
		list<int> diaoyong;
		while (canshu_diaoyong.back() != 8) {
			diaoyong.push_front(canshu_diaoyong.back());
			canshu_diaoyong.pop_back();
		}
		canshu_diaoyong.pop_back();  // 去掉8

		if (dingyi.size() != diaoyong.size()) {
			error(0, 'd');
		}
		else {
			while (dingyi.size() > 0) {
				if ((diaoyong.back() % 2) != (dingyi.back() % 2)) {
					error(0, 'e');
					break;
				}
				diaoyong.pop_back();
				dingyi.pop_back();
			}
		}
		Siyuanshi siyuanshi("call_end", funcName_diaoyong.front(), NULL_CHAR, NULL_CHAR);
		siyuanshi_list.push_front(siyuanshi);
		funcName_diaoyong.pop_front();
	}
	else {
		assert(type == 2);
		assert(fuhao != 8);
		canshu_diaoyong.push_back(fuhao);
	}
}

list<int> find_label(string name, int print) {
	if (name == "") {
		name = q.front().substr(7, q.front().length() - 7);
	}
	if (local_labels.count(name) > 0) {
		return local_labels[name];
	}
	else {
		if (global_labels.count(name)) {
			return global_labels[name];
		}
		else {
			if (print == 1) {
				error(-1, 'c');
			}
			return list<int>{ 0 }; // 返回含一个0元素的list,防止空list
		}
	}

}

int insert_label(string name, int type, list<int> canshu = list<int>(0)) {
	if (DEBUG_MIDDLE == 1) {
		cout << "insert_label isLocalFunc: " << isLocalFunc << endl;
	}
	if (isLocalFunc == 0) {
		if (global_labels.count(name) > 0) {
			error(-1, 'b');
		}
		else {
			if (type != 8) {
				canshu.push_front(type);
			}
			global_labels.insert(pair<string, list<int>>(name, canshu));
		}
	}
	else {
		if (local_labels.count(name) > 0) {
			error(-1, 'b');
		}
		else {
			if (type != 8) {
				canshu.push_front(type);
			}
			local_labels.insert(pair<string, list<int>>(name, canshu));
		}
	}
	return 0;
}

// fuhao: 0=默认, 1=const int, 2=const char, 3=int, 4=char, 5=void func, 6=int func, 7=char func,
//		  8=end of canshubiao, 12=const(int\char)
void judge(int sym, int a, char type, int fuhao) {
	if (sym != a) {
		if (a == 28) {
			// 特判: ;缺失导致先输出错误信息,再行号加一
			if (type == 'K') {
				error(a, 'k');
			}
			else {
				error(a, 'k', LINE_NUM_LAST);
			}
		}
		else if (a == 31) {
			error(a, 'l');
		}
		else if (a == 33) {
			error(a, 'm');
		}
		// 下面的是由于不是所有不等于的情况都要报错(比如 n , o, i ),但要除掉b,c,K
		if (type != '-' && type != 'b' && type != 'c' && type != 'K') {
			error(a, type);
			if (type == 'o' || (type == 'i' && sym != 33)) {
				LIST.pop_front();
				q.pop();
			}
		}
	}
	else {
		if (type == 'b') {
			assert(sym == 0);
			string name = q.front().substr(7, q.front().length() - 7);
			if (fuhao == 5 || fuhao == 6 || fuhao == 7) {
				assert(canshu_dingyi.size() == 0);
				assert(isLocalFunc == 0);
				canshu_dingyi.push_back(fuhao);  // 函数的canshu第一个是函数的type
				funcName_dingyi = name;
				Siyuanshi siyuanshi("hanshu", name, NULL_CHAR, NULL_CHAR, to_string(fuhao), "0", "0");
				siyuanshi_list.push_front(siyuanshi);
			}
			else if (fuhao == 8) {
				// 该judge是新加的,推进栈中后,立刻返回
				assert(canshu_dingyi.size() > 0);
				assert(isLocalFunc == 0);
				insert_label(funcName_dingyi, fuhao, canshu_dingyi);
				canshu_dingyi.clear();
				return;
			}
			// 定义时使用isLocalFunc判断没问题
			else if (canshu_dingyi.size() > 0 && isLocalFunc > 0) {
				assert(isLocalFunc > 0);
				canshu_dingyi.push_back(fuhao);
				insert_label(name, fuhao);
				Siyuanshi siyuanshi("para", name, NULL_CHAR, NULL_CHAR, to_string(fuhao) + " 1", "0", "0");
				siyuanshi_list.push_front(siyuanshi);
			}
			else {
				insert_label(name, fuhao);
				add_iden(0, fuhao);
				Siyuanshi siyuanshi("var", name, NULL_CHAR, NULL_CHAR, to_string(fuhao) + " " + to_string(isLocalFunc), "0", "0");
				siyuanshi_list.push_front(siyuanshi);
			}
		}
		else if (type == 'c') {
			assert(sym == 0);
			string name = q.front().substr(7, q.front().length() - 7);
			list<int> fuhaos_tmp = find_label(name);
			int fuhao_tmp = fuhaos_tmp.front();
			if (fuhao == 12 && (fuhao_tmp == 1 || fuhao_tmp == 2)) {
				error(-1, 'j');
			}
		}
		tofo(q.front());
		LIST.pop_front();
		q.pop();
	}
}
