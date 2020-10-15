#include "my.h"

list<string> sym_list;
list<string> type_list;
list<string> tmp_reg_list;
int tmp_reg_start = 0;
list<string> tmp_label_list;
int tmp_label_start = 0;
list<string> tmp_str_list;
int tmp_str_start = 0;

Siyuanshi::Siyuanshi(string op_, string result_, string leftnum_, string rightnum_,
	string result_type_, string left_type_, string right_type_) {
	op = op_;
	result = result_;
	leftnum = leftnum_;
	rightnum = rightnum_;
	result_type = result_type_;
	left_type = left_type_;
	right_type = right_type_;
}

string Siyuanshi::output_string() {
	if (op == ".asciiz") {
		return result + ": " + op + " " + leftnum;
	}
	if (op == "=") {
		return result + " " + op + " " + leftnum;
	}
	if (op == "=retval") {
		return op + " $v1 = " + result;
	}
	if (op == "ret" || op == "inline_clear" || 
		op == "for_start" || op == "while_start" || op == "do_start") {
		return op;
	}
	if (op == "ret()" || op == "push" || op == "print" || op == "scanf" || op == "set_label"
		|| op == "jump" || op == "call_end" || op == "call_start") {
		return op + " " + result;
	}
	if (op == "BZ_jump" || op == "BNZ_jump" || op == "print2") {
		return op + " " + leftnum + " " + result;
	}
	if (op == "var" || op == "para") {
		if (result_type[0] == '1') {
			return op + " const int " + result;
		}
		else if (result_type[0] == '2') {
			return op + " const char " + result;
		}
		else if (result_type[0] == '3') {
			return op + " int " + result;
		}
		else if (result_type[0] == '4') {
			return op + " char " + result;
		}
	}
	if (op == "var[]") {
		if (result_type[0] == '1') {
			return op + " const int " + result + "[" + leftnum + "]";
		}
		else if (result_type[0] == '2') {
			return op + " const char " + result + "[" + leftnum + "]";
		}
		else if (result_type[0] == '3') {
			return op + " int " + result + "[" + leftnum + "]";
		}
		else if (result_type[0] == '4') {
			return op + " char " + result + "[" + leftnum + "]";
		}
	}
	if (op == "hanshu") {
		if (result_type == "5") {
			return "void " + result + "()";
		}
		else if (result_type == "6") {
			return "int " + result + "()";
		}
		else if (result_type == "7") {
			return "char " + result + "()";
		}
	}
	if (op == "--") {
		return result + " = -" + leftnum;
	}
	if (op == "[]store") {
		return op + " " + result + "[" + leftnum + "] = " + rightnum;
	}
	if (op == "[]load") {
		return op + " " + result + " = " + leftnum + "[" + rightnum + "]";
	}
	//  op == "+" || op == "-" || op == "*" || op == "/" ||
	// 	op == "==" || op == "!=" || op == ">=" || op == ">" || op == "<=" || op == "<"
	return result + " = " + leftnum + " " + op + " " + rightnum;
}

// type: 0=变量, 1=纯int(5), 2=纯char(6), 3=纯str(7), 4=int return, 5=char return
void add_iden(int type, int iden_type) {
	if (IS_ANY_ERROR != 0) {
		return;
	}
	if (DEBUG_MIDDLE == 1) {
		cout << endl << "enter add_iden type: " << type << endl;
		if (sym_list.size() > 0) {
			cout << "add_iden: " << sym_list.size() << " " << sym_list.front() << endl;
			cout << "add_iden: " << type_list.size() << " " << type_list.front() << endl;
		}
	}
	assert(sym_list.size() == type_list.size());
	if (type == 0) {
		string iden = q.front().substr(7, q.front().length() - 7);
		if (iden_type == 0) {
			list<int> tmp = find_label(iden);
			iden_type = tmp.front();
		}
		sym_list.push_front(iden);
		// 调用时使用isLocalFunc判断有问题, 所以改成如下判断
		if (global_labels.count(iden) > 0 && local_labels.count(iden) == 0) {
			type_list.push_front(to_string(iden_type) + " 0");
		}
		else {
			type_list.push_front(to_string(iden_type) + " " + to_string(isLocalFunc));
		}
	}
	else if (type == 1) {
		string num = q.front().substr(7, q.front().length() - 7);
		sym_list.push_front(num);
		type_list.push_front("5");
	}
	else if (type == 2) {
		string char_ = q.front().substr(8, q.front().length() - 8);
		sym_list.push_front("'" + char_ + "'");
		type_list.push_front("6");
	}
	else if (type == 3) {
		string str = q.front().substr(7, q.front().length() - 7);
		int len = str.length();
		for (int i = 0; i < len; i++) {
			if (str[i] == '\\') {
				str.insert(i, "\\");
				len++;
				i++;
			}
		}
		str = "\"" + str + "\"";
		sym_list.push_front(str);
		type_list.push_front("7");
	}
	else if (type == 4 || type == 5) {
		string result = tmp_reg_list.front();
		tmp_reg_list.pop_front();
		if (type == 4) {
			Siyuanshi siyuanshi("=", result, "#ret_val", NULL_CHAR, "8", "9", "0");
			siyuanshi_list.push_front(siyuanshi);
		}
		else {
			Siyuanshi siyuanshi("=", result, "#ret_val", NULL_CHAR, "8", "10", "0");
			siyuanshi_list.push_front(siyuanshi);
		}
		sym_list.push_front(result);
		if (type == 4) {
			type_list.push_front("8");
		}
		else {
			type_list.push_front("8" + to_string(1));
		}
	}
	if (DEBUG_MIDDLE == 1) {
		if (sym_list.size() > 0) {
			cout << "out add_iden: sym_list size: " << sym_list.size() << " front: " << sym_list.front() << endl;
			cout << "out add_iden: type_list size: " << type_list.size() << " front: " << type_list.front() << endl;
		}
		cout << "out add_iden" << endl << endl;
	}
}

string add_op(string op, string label_input) {
	if (IS_ANY_ERROR != 0) {
		return "";
	}
	if (DEBUG_MIDDLE == 1) {
		cout << endl << "enter add_op: " << op << endl;
		if (sym_list.size() > 0) {
			cout << "add_op: sym_list size: " << sym_list.size() << " front: " << sym_list.front() << endl;
			cout << "add_op: type_list size:  " << type_list.size() << " front: " << type_list.front() << endl;
		}
	}
	assert(sym_list.size() == type_list.size());
	string return_str;
	if (tmp_reg_list.size() < 20) {
		for (int i = tmp_reg_start; i < tmp_reg_start + 100; i++) {
			tmp_reg_list.push_back("$reg_" + to_string(i));
		}
		tmp_reg_start += 200;
	}
	if (tmp_label_list.size() < 10) {
		for (int i = tmp_label_start; i < tmp_label_start + 50; i++) {
			tmp_label_list.push_back("$label_" + to_string(i));
		}
		tmp_label_start += 100;
	}
	if (tmp_str_list.size() < 5) {
		for (int i = tmp_str_start; i < tmp_str_start + 25; i++) {
			tmp_str_list.push_back("$str_" + to_string(i));
		}
		tmp_str_start += 50;
	}
	if (DEBUG_MIDDLE == 1) {
		cout << "op" << op << endl;
	}
	if (op == "+" || op == "-" || op == "*" || op == "/" ||
		op == "==" || op == "!=" || op == ">=" || op == ">" || op == "<=" || op == "<") {
		assert(sym_list.size() >= 2);
		// cout << sym_list.size() << endl;
		// cout << type_list.size() << endl;
		string a = sym_list.front();
		sym_list.pop_front();
		string a_type = type_list.front();
		type_list.pop_front();
		string b = sym_list.front();
		sym_list.pop_front();
		string b_type = type_list.front();
		type_list.pop_front();
		string result = tmp_reg_list.front();
		tmp_reg_list.pop_front();
		Siyuanshi siyuanshi(op, result, b, a, "8", b_type, a_type);
		siyuanshi_list.push_front(siyuanshi);
		sym_list.push_front(result);
		type_list.push_front("8");
	}
	// "--" 表示'-'在开头的情况 "-xxx", "++"不需要
	else if (op == "--") {
		assert(sym_list.size() >= 1);
		string a = sym_list.front();
		sym_list.pop_front();
		string a_type = type_list.front();
		type_list.pop_front();
		string result = tmp_reg_list.front();
		tmp_reg_list.pop_front();
		Siyuanshi siyuanshi(op, result, a, NULL_CHAR, "8", a_type, "0");
		siyuanshi_list.push_front(siyuanshi);
		sym_list.push_front(result);
		type_list.push_front("8");
	}
	else if (op == "=") {
		assert(sym_list.size() >= 2);
		string a = sym_list.front();
		sym_list.pop_front();
		string a_type = type_list.front();
		type_list.pop_front();
		string b = sym_list.front();
		sym_list.pop_front();
		string b_type = type_list.front();
		type_list.pop_front();
		if (b_type.size() >= 2 && b_type[1] == '1') {
			string result = tmp_reg_list.front();
			tmp_reg_list.pop_front();
			Siyuanshi siyuanshi(op, result, a, NULL_CHAR, "8", a_type, "0");
			siyuanshi_list.push_front(siyuanshi);
			string c = sym_list.front();
			sym_list.pop_front();
			string c_type = type_list.front();
			type_list.pop_front();
			Siyuanshi siyuanshi_2("[]store", c, b, result, c_type, b_type, "8");
			siyuanshi_list.push_front(siyuanshi_2);
		}
		else {
			Siyuanshi siyuanshi(op, b, a, NULL_CHAR, b_type, a_type, "0");
			siyuanshi_list.push_front(siyuanshi);
		}
	}
	else if (op == "[]load") {
		assert(sym_list.size() >= 1);
		string a = sym_list.front();
		sym_list.pop_front();
		string a_type = type_list.front();
		type_list.pop_front();
		string b = sym_list.front();
		sym_list.pop_front();
		string b_type = type_list.front();
		type_list.pop_front();
		string result = tmp_reg_list.front();
		tmp_reg_list.pop_front();
		Siyuanshi siyuanshi(op, result, b, a, "8", b_type, a_type);
		siyuanshi_list.push_front(siyuanshi);
		sym_list.push_front(result);
		if (b_type[0] == '3') {
			type_list.push_front("8");
		}
		else {
			assert(b_type[0] == '4');
			type_list.push_front("8" + to_string(1));
		}

	}
	else if (op == "[]store") {
		assert(sym_list.size() >= 2);
		if (DEBUG_MIDDLE == 1) {
			cout << "[]store" << type_list.front() << endl;
		}
		if (type_list.front().size() == 1) {
			type_list.front() = type_list.front() + "1";
		}
		else {
			type_list.front()[1] = '1';
		}
		if (DEBUG_MIDDLE == 1) {
			cout << "[]store" << type_list.front() << endl;
		}
		return "";
	}
	else if (op == "[]init") {
		assert(sym_list.size() >= 1);
		string a = sym_list.front();
		sym_list.pop_front();
		string a_type = type_list.front();
		type_list.pop_front();
		assert(siyuanshi_list.front().op == "var");
		siyuanshi_list.front().op = "var[]";
		siyuanshi_list.front().leftnum = a;
		siyuanshi_list.front().left_type = a_type;
	}
	else if (op == "()") {
		assert(sym_list.size() >= 1);
		if (type_list.front()[0] != '2' && type_list.front()[0] != '4' && type_list.front()[0] != '6') {
			return "";
		}
		string a = sym_list.front();
		sym_list.pop_front();
		string a_type = type_list.front();
		type_list.pop_front();
		string result = tmp_reg_list.front();
		tmp_reg_list.pop_front();
		Siyuanshi siyuanshi("=", result, a, NULL_CHAR, "8", a_type, "0");
		siyuanshi_list.push_front(siyuanshi);
		sym_list.push_front(result);
		type_list.push_front("8");
	}
	else if (op == "ret()") {
		assert(sym_list.size() >= 1);
		string a = sym_list.front();
		sym_list.pop_front();
		string a_type = type_list.front();
		type_list.pop_front();
		Siyuanshi siyuanshi(op, a, NULL_CHAR, NULL_CHAR, a_type, "0", "0");
		siyuanshi_list.push_front(siyuanshi);
	}
	else if (op == "ret") {
		Siyuanshi siyuanshi(op, NULL_CHAR, NULL_CHAR, NULL_CHAR);
		siyuanshi_list.push_front(siyuanshi);
	}
	else if (op == "optimal") {
		Siyuanshi siyuanshi(label_input, NULL_CHAR, NULL_CHAR, NULL_CHAR);
		siyuanshi_list.push_front(siyuanshi);
	}
	else if (op == "push" || op == "print" || op == "scanf") {
		assert(sym_list.size() >= 1);
		assert(type_list.size() >= 1);
		string a = sym_list.front();
		sym_list.pop_front();
		string a_type = type_list.front();
		type_list.pop_front();

		if (op == "print" && a_type == "7") {
			string label = tmp_str_list.front();
			tmp_str_list.pop_front();
			Siyuanshi siyuanshi_data(".asciiz", label, a, NULL_CHAR);
			siyuanshi_data_list.push_front(siyuanshi_data);
			Siyuanshi siyuanshi(op, label, NULL_CHAR, NULL_CHAR, a_type, "0", "0");
			siyuanshi_list.push_front(siyuanshi);
		}
		else {
			Siyuanshi siyuanshi(op, a, NULL_CHAR, NULL_CHAR, a_type, "0", "0");
			siyuanshi_list.push_front(siyuanshi);
		}
	}
	else if (op == "print2") {
		assert(sym_list.size() >= 1);
		string a = sym_list.front();
		sym_list.pop_front();
		string a_type = type_list.front();
		type_list.pop_front();
		string b = sym_list.front();
		sym_list.pop_front();
		string b_type = type_list.front();
		type_list.pop_front();
		// a 是int或者char, b是str
		string label = tmp_str_list.front();
		tmp_str_list.pop_front();
		Siyuanshi siyuanshi_data(".asciiz", label, b, NULL_CHAR);
		siyuanshi_data_list.push_front(siyuanshi_data);
		Siyuanshi siyuanshi("print2", a, label, NULL_CHAR, a_type, b_type, "0");
		siyuanshi_list.push_front(siyuanshi);
	}
	else if (op == "set_label") {
		if (label_input == "") {
			label_input = tmp_label_list.front();
			tmp_label_list.pop_front();
		}
		Siyuanshi siyuanshi(op, label_input, NULL_CHAR, NULL_CHAR);
		siyuanshi_list.push_front(siyuanshi);
	}
	else if (op == "set_xunhuan_start_label") {
		string label = tmp_label_list.front();
		tmp_label_list.pop_front();
		Siyuanshi siyuanshi("set_label", label, NULL_CHAR, NULL_CHAR);
		siyuanshi_list.push_front(siyuanshi);
		return_str = label;
	}
	else if (op == "jump") {
		if (label_input == "") {
			label_input = tmp_label_list.front();
			tmp_label_list.pop_front();
		}
		Siyuanshi siyuanshi(op, label_input, NULL_CHAR, NULL_CHAR);
		siyuanshi_list.push_front(siyuanshi);
		return_str = label_input;
	}
	else if (op == "BZ_jump" || op == "BNZ_jump") {
		assert(sym_list.size() >= 1);
		string a = sym_list.front();
		sym_list.pop_front();
		string a_type = type_list.front();
		type_list.pop_front();
		if (label_input == "") {
			label_input = tmp_label_list.front();
			tmp_label_list.pop_front();
		}
		Siyuanshi siyuanshi(op, label_input, a, NULL_CHAR, "0", a_type, "0");
		siyuanshi_list.push_front(siyuanshi);
		return_str = label_input;
	}
	if (DEBUG_MIDDLE == 1) {
		cout << siyuanshi_list.front().output_string() << endl;
	}
	return return_str;
}
