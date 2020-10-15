#include "my.h"

map<string, string> local_name_offset;
int offset_end = 0;
map<string, string> global_name_offset;
int offset_global_array = 0;
map<string, string> name_s_reg_map;
map<string, string> s_reg_addr_map;
list<string> s_regs; 
map<string, string> name_t_reg_map;
map<string, string> t_reg_addr_map;  // int是1的话,说明是空的(正常地址都是4的倍数)
// 在基本块的入口处(set_label处 + jump处 + 初始化)清空(int = 1)map
list<string> t_regs;  // LRU算法,栈顶是最近使用的.栈底弹出时看t_reg_map是否为空
list<string> t_regs_89; 
list<int> push_index;
list<string> inline_regs;

void clear_s() {
	name_s_reg_map.clear();
	for (int i = 0; i <= 7; i++) {
		s_reg_addr_map["$s" + to_string(i)] = "1";
	}
	s_reg_addr_map["$a1"] = "1";
	s_reg_addr_map["$a2"] = "1";
	s_reg_addr_map["$a3"] = "1";
	s_reg_addr_map["$k0"] = "1";
	s_regs.clear();
	for (int i = 0; i <= 7; i++) {
		s_regs.push_front("$s" + to_string(i));
	}
	s_regs.push_front("$a1");
	s_regs.push_front("$a2");
	s_regs.push_front("$a3");
	s_regs.push_front("$k0");
}

void clear_t() {
	name_t_reg_map.clear();
	t_regs_89.clear();
	t_regs_89.push_front("$t8");
	t_regs_89.push_front("$t9");
	// $k1(用于全局数组), $t8, $t9备用 用于纯数字\纯字符\返回值等的处理
	for (int i = 0; i <= 7; i++) {
		t_reg_addr_map["$t" + to_string(i)] = "1";
	}
	t_regs.clear();
	for (int i = 0; i <= 7; i++) {
		t_regs.push_front("$t" + to_string(i));
	}
}

void refresh_reg(string name, string type, int inline_reg = 0) {
	if (name != NULL_CHAR && type[0] == '8' && name_t_reg_map.count(name) > 0) {
		if (name[1] != 'r' && inline_reg == 0) {
			assert(name[1] == 'i');
			inline_regs.push_front(name);
			return;
		}
		string reg = name_t_reg_map[name];
		if (DEBUG_MIPS == 1) {
			cout << "name: " << name << " refresh reg: " << reg << endl;
		}
		t_reg_addr_map[reg] = "1";
		for (map<string, string>::iterator iter = name_t_reg_map.begin(); iter != name_t_reg_map.end(); ) {
			string tmp = iter->first;
			if (tmp == name) {
				name_t_reg_map.erase(iter++);
			}
			else {
				iter++;
			}
		}
		for (list<string>::iterator iter = t_regs.begin(); iter != t_regs.end(); ) {
			if (*iter != reg) {
				iter++;
			}
			else {
				t_regs.erase(iter++);
			}
		}
		t_regs.push_back(reg);
	}
}

string find_reg(string name, string type) {
	if (DEBUG_MIPS == 1) {
		cout << "find_reg name: " << name << " type: " << type << endl;
	}
	if (type[0] == '8' && local_name_offset.count(name) == 0) {
		local_name_offset[name] = to_string(offset_end);
		offset_end -= 4;
	}
	string reg;
	if (name_t_reg_map.count(name) > 0) {
		reg = name_t_reg_map[name];
		for (list<string>::iterator iter = t_regs.begin(); iter != t_regs.end(); ) {
			if (*iter != reg) {
				iter++;
			}
			else {
				t_regs.erase(iter++);
			}
		}
		t_regs.push_front(reg);
	}
	else if (name_s_reg_map.count(name) > 0) {
		reg = name_s_reg_map[name];
		for (list<string>::iterator iter = s_regs.begin(); iter != s_regs.end(); ) {
			if (*iter != reg) {
				iter++;
			}
			else {
				s_regs.erase(iter++);
			}
		}
		s_regs.push_front(reg);
	}
	else
	{
		if (type[0] == '8') {
			reg = t_regs.back();
			t_regs.pop_back();
			if (t_reg_addr_map[reg] != "1") {
				if (DEBUG_MIPS == 1) {
					cout << "conflict t_reg: " << reg << endl;
				}
				// 两个空格的sw表示冲突
				tofmips("  sw    " + reg + ",    " + t_reg_addr_map[reg] + "($sp)");
			}
			t_reg_addr_map[reg] = local_name_offset[name];
			name_t_reg_map[name] = reg;
			if (DEBUG_MIPS == 1) {
				cout << "t_reg_addr_map[reg]: " << t_reg_addr_map[reg] << " reg: " << reg << endl;
				cout << "name: " << name << " name_t_reg_map[name]: " << name_t_reg_map[name] << endl;
			}
			t_regs.push_front(reg);
		}
		else {
			reg = s_regs.back();
			s_regs.pop_back();
			if (s_reg_addr_map[reg] != "1") {
				if (DEBUG_MIPS == 1) {
					cout << "conflict s_reg: " << reg << endl;
				}
				// 两个空格的sw表示冲突
				tofmips("  sw    " + reg + ",    " + s_reg_addr_map[reg] + "($sp)");
				// 删除没有寄存器的变量名字,防止再次寻找时,误认为有
				for (map<string, string>::iterator iter = name_s_reg_map.begin(); iter != name_s_reg_map.end(); ) {
					string tmp = iter->second;
					if (tmp == reg) {
						name_s_reg_map.erase(iter++);
					}
					else {
						iter++;
					}
				}
			}
			s_reg_addr_map[reg] = local_name_offset[name];
			name_s_reg_map[name] = reg;
			if (DEBUG_MIPS == 1) {
				cout << "s_reg_addr_map[reg]: " << s_reg_addr_map[reg] << " reg: " << reg << endl;
				cout << "name: " << name << " name_s_reg_map[name]: " << name_s_reg_map[name] << endl;
			}
			s_regs.push_front(reg);
		}
	}
	return reg;
}

string order_lw(string source, string type, string aim = "") {
	string str;
	if (DEBUG_MIPS == 1) {
		cout << "order_lw source: " << source << " type: " << type << endl;
	}
	if (aim == "") {
		if (type[0] == '5' || type == "6" || type == "9" || type == "10" || (type.size() == 3 && type[2] == '0')) {
			aim = t_regs_89.back();
			t_regs_89.pop_back();
			t_regs_89.push_front(aim);
		}
		else {
			if (name_t_reg_map.count(source) > 0 || name_s_reg_map.count(source) > 0) {
				// 必须要在find_t_reg之前判断,因为函数里改变了map
				aim = find_reg(source, type);
				return aim;
			}
			else {
				aim = find_reg(source, type);
				// return aim;
			}			
		}
	}
	if (type[0] == '5') {
		str = "    li    " + aim + ",    " + source;
	}
	else if (type[0] == '6') {
		str = "    li    " + aim + ",    " + source;
	}
	else if (type == "9" || type == "10") {
		str = "    move  " + aim + ",    $v1";
	}
	else if (type.length() == 3 && type[2] == '0') {
		str = "    lw    " + aim + ",    " + source;
	}
	else if (type[0] == '8' && (aim != name_t_reg_map[source])) {
		str = "    move  " + aim + ",    " + name_t_reg_map[source];
	}
	else if(name_s_reg_map.count(source) > 0 && (aim != name_s_reg_map[source])) {
		str = "    move  " + aim + ",    " + name_s_reg_map[source];
	}
	else {
		str = "    lw    " + aim + ",    " + local_name_offset[source] + "($sp)";
	}
	tofmips(str);
	return aim;
}

string order_sw(Siyuanshi siyuanshi, string source) {
	string str;
	if (siyuanshi.result_type[0] == '8' && local_name_offset.count(siyuanshi.result) == 0) {
		local_name_offset[siyuanshi.result] = to_string(offset_end);
		offset_end -= 4;
	}
	if (siyuanshi.result_type.length() == 3 && siyuanshi.result_type[2] == '0') {
		str = "    sw    " + source + ",    " + siyuanshi.result;
	}
	else {
		// TODO: 需要下面的指令,但想想为什么
		if (name_s_reg_map.count(siyuanshi.result) > 0 && siyuanshi.result_type[0] != '8') {
			str = "    move  " + name_s_reg_map[siyuanshi.result] + ",    " + source;
		}
		else {
			string aim = find_reg(siyuanshi.result, siyuanshi.result_type);
			str = "    move  " + aim + ",    " + source;
		}
	}
	tofmips(str);
	return str;
}

void save_environment() {
	for (int i = 0; i <= 7 ; i++) {
		string reg = "$s" + to_string(i);
		if (DEBUG_MIPS == 1) {
			cout << "save environment: " << s_reg_addr_map[reg] << endl;
		}
		if (s_reg_addr_map[reg] != "1") {
			tofmips("    sw    " + reg + ",    " + s_reg_addr_map[reg] + "($sp)");
		}
	}
	for (int i = 1; i <= 3; i++) {
		string reg = "$a" + to_string(i);
		if (DEBUG_MIPS == 1) {
			cout << "save environment: " << s_reg_addr_map[reg] << endl;
		}
		if (s_reg_addr_map[reg] != "1") {
			tofmips("    sw    " + reg + ",    " + s_reg_addr_map[reg] + "($sp)");
		}
	}
	tofmips("    addi  $sp,    $sp,    " + to_string(offset_end - 8));
	tofmips("    sw    $ra,    8($sp)");
	tofmips("    sw    $fp,    4($sp)");
	tofmips("    move  $fp,    $sp");
}

void recover_environment() {
	tofmips("    move  $sp,    $fp");
	tofmips("    lw    $fp,    4($sp)");
	tofmips("    lw    $ra,    8($sp)");
	tofmips("    addi  $sp,    $sp,    " + to_string(8 - offset_end));
	for (int i = 0; i <= 7; i++) {
		string reg = "$s" + to_string(i);
		if (DEBUG_MIPS == 1) {
			cout << "save environment: " << s_reg_addr_map[reg] << endl;
		}
		if (s_reg_addr_map[reg] != "1") {
			tofmips("    lw    " + reg + ",    " + s_reg_addr_map[reg] + "($sp)");
		}
	}
	for (int i = 1; i <= 3; i++) {
		string reg = "$a" + to_string(i);
		if (DEBUG_MIPS == 1) {
			cout << "save environment: " << s_reg_addr_map[reg] << endl;
		}
		if (s_reg_addr_map[reg] != "1") {
			tofmips("    sw    " + reg + ",    " + s_reg_addr_map[reg] + "($sp)");
		}
	}
}

void generate_mips() {
	clear_t();
	clear_s();
	tomiddle(".data");
	tofmips(".data");
	tofmips("enter:     .asciiz \"\\n\"");
	while (siyuanshi_data_list.size() > 0) {
		Siyuanshi siyuanshi = siyuanshi_data_list.back();
		assert(siyuanshi.op == ".asciiz");
		tofmips(siyuanshi.result + ":     " + siyuanshi.op + " " + siyuanshi.leftnum);
		siyuanshi_data_list.pop_back();
	}
	while (1) {
		Siyuanshi siyuanshi = siyuanshi_list.back();
		if (siyuanshi.op == "hanshu") {
			break;
		}
		else if (siyuanshi.op == "var" && siyuanshi.result_type.length() == 3 && siyuanshi.result_type[2] == '0') {
			string str = siyuanshi.result + ":    .word ";
			if (siyuanshi.result_type[0] == '1') {
				tomiddle(siyuanshi_list.back().output_string());
				siyuanshi_list.pop_back();
				siyuanshi = siyuanshi_list.back();
				if (siyuanshi.op == "--") {
					string num = siyuanshi.leftnum;
					tomiddle(siyuanshi_list.back().output_string());
					str += "-" + num;
				}
				else {
					str += siyuanshi.leftnum;
				}
			}
			else if (siyuanshi.result_type[0] == '2') {
				tomiddle(siyuanshi_list.back().output_string());
				siyuanshi_list.pop_back();
				siyuanshi = siyuanshi_list.back();
				str += siyuanshi.leftnum;
			}
			else {
				str += "0";
			}
			tofmips(str);
		}
		else if (siyuanshi.op == "var[]" && siyuanshi.result_type.length() == 3 && siyuanshi.result_type[2] == '0') {
			global_name_offset[siyuanshi.result] = to_string(offset_global_array);
			offset_global_array -= 4 * stoi(siyuanshi.leftnum);
			if (DEBUG_MIPS == 1) {
				cout << "offset_global_array[]: " << offset_global_array << endl;
			}
		}
		else {
			if (DEBUG_ERROR == 1) {
				cout << "!!!!!!!!!!!!!!!!!!!!!!! op wrong: " << siyuanshi.op << endl;
			}
		}
		tomiddle(siyuanshi_list.back().output_string());
		siyuanshi_list.pop_back();
	}
	tomiddle("");
	tomiddle(".text");
	tofmips("");
	tofmips(".text");
	tofmips("    lui   $k1,    0x1004");
	tofmips("    jal   main");
	tofmips("    j     $END");
	while (siyuanshi_list.size() > 0) {
		Siyuanshi siyuanshi = siyuanshi_list.back();
		string str;
		string str_1;
		string str_2;
		string str_3;
		if (siyuanshi.op == "+") {
			str_3 = find_reg(siyuanshi.result, siyuanshi.result_type);
			if (siyuanshi.left_type[0] == '5' && siyuanshi.right_type[0] == '5') {
				int left = stoi(siyuanshi.leftnum);
				int right = stoi(siyuanshi.rightnum);
				tofmips("    addi  " + str_3 + ",    $0,    " + to_string(left + right));
			}
			else if (siyuanshi.left_type[0] == '5') {
				int left = stoi(siyuanshi.leftnum);
				str_2 = order_lw(siyuanshi.rightnum, siyuanshi.right_type, "");
				tofmips("    addi  " + str_3 + ",    " + str_2 + ",    " + to_string(left));
			}
			else if (siyuanshi.right_type[0] == '5') {
				int right = stoi(siyuanshi.rightnum);
				str_1 = order_lw(siyuanshi.leftnum, siyuanshi.left_type, "");
				tofmips("    addi  " + str_3 + ",    " + str_1 + ",    " + to_string(right));
			}
			else {
				str_1 = order_lw(siyuanshi.leftnum, siyuanshi.left_type, "");
				str_2 = order_lw(siyuanshi.rightnum, siyuanshi.right_type, "");
				tofmips("    add   " + str_3 + ",    " + str_1 + ",    " + str_2);
			}
		}
		else if (siyuanshi.op == "-") {
			str_3 = find_reg(siyuanshi.result, siyuanshi.result_type);
			if (siyuanshi.left_type[0] == '5' && siyuanshi.right_type[0] == '5') {
				int left = stoi(siyuanshi.leftnum);
				int right = stoi(siyuanshi.rightnum);
				// 用addi少一条指令,所以左右倒过来
				tofmips("    addi  " + str_3 + ",    $0,    " + to_string(right - left));
			}
			else if (siyuanshi.right_type[0] == '5') {
				int right = stoi(siyuanshi.rightnum);
				str_1 = order_lw(siyuanshi.leftnum, siyuanshi.left_type, "");
				if (right >= 0) {
					tofmips("    addi  " + str_3 + ",    " + str_1 + ",    -" + to_string(right));
				}
				else {
					right = -right;
					tofmips("    addi  " + str_3 + ",    " + str_1 + ",    " + to_string(right));
				}
			}
			else {
				str_1 = order_lw(siyuanshi.leftnum, siyuanshi.left_type, "");
				str_2 = order_lw(siyuanshi.rightnum, siyuanshi.right_type, "");
				tofmips("    sub   " + str_3 + ",    " + str_1 + ",    " + str_2);
			}
		}
		else if (siyuanshi.op == "--") {
			str_1 = order_lw(siyuanshi.leftnum, siyuanshi.left_type);
			str_3 = find_reg(siyuanshi.result, siyuanshi.result_type);
			tofmips("    sub   " + str_3 + ",    $0,    " + str_1);
		}
		else if (siyuanshi.op == "*") {
			str_1 = order_lw(siyuanshi.leftnum, siyuanshi.left_type);
			str_2 = order_lw(siyuanshi.rightnum, siyuanshi.right_type); 
			str_3 = find_reg(siyuanshi.result, siyuanshi.result_type);
			tofmips("    mul   " + str_3 + ",    " + str_1 + ",    " + str_2);  // mul 只有一条mips指令,mult + mflo两条
		}
		else if (siyuanshi.op == "/") {
			str_1 = order_lw(siyuanshi.leftnum, siyuanshi.left_type);
			str_2 = order_lw(siyuanshi.rightnum, siyuanshi.right_type);
			tofmips("    div   " + str_1 + ",    " + str_2);  // div $a, $b, $c 有四条mips指令(判断$c是否为0)
			// result 也是int,所以不用考虑HI寄存器(余数)
			str_3 = find_reg(siyuanshi.result, siyuanshi.result_type);
			tofmips("    mflo  " + str_3);
		}
		else if (siyuanshi.op == "=") {
			if (siyuanshi.result_type[0] == '8') {
				str_1 = order_lw(siyuanshi.leftnum, siyuanshi.left_type);
				str_3 = find_reg(siyuanshi.result, siyuanshi.result_type);
				tofmips("    move  " + str_3 + ",    " + str_1);
			}
			else if (siyuanshi.result_type[0] == '5') {
				str_3 = find_reg(siyuanshi.result, siyuanshi.result_type);
				tofmips("    li    " + str_3 + ",    " + siyuanshi.result);
			}
			else {
				str_1 = order_lw(siyuanshi.leftnum, siyuanshi.left_type);
				order_sw(siyuanshi, str_1);
			}
		}
		else if (siyuanshi.op == "=retval") {
			str_1 = order_lw(siyuanshi.result, siyuanshi.result_type);
			tofmips("    move  $v1,    " + str_1);
		}
		else if (siyuanshi.op == "==") {
			if (siyuanshi.leftnum != "0") {
				str_1 = order_lw(siyuanshi.leftnum, siyuanshi.left_type);
			}
			else {
				str_1 = "0";
			}
			if (siyuanshi.rightnum != "0") {
				str_2 = order_lw(siyuanshi.rightnum, siyuanshi.right_type);
			}
			else {
				str_2 = "0";
			}
			tomiddle(siyuanshi_list.back().output_string());
			siyuanshi_list.pop_back();
			siyuanshi = siyuanshi_list.back();
			if (siyuanshi.op == "BZ_jump") {
				if (str_1 == "0") {
					tofmips("    beq   $0,    " + str_2 + ",    " + siyuanshi.result);
				}
				else if (str_2 == "0") {
					tofmips("    beq   " + str_1 + ",    $0,    " + siyuanshi.result);
				}
				else {
					tofmips("    beq   " + str_1 + ",    " + str_2 + ",    " + siyuanshi.result);
				}
			}
			else {
				assert(siyuanshi.op == "BNZ_jump");
				if (str_1 == "0") {
					tofmips("    bne   $0,    " + str_2 + ",    " + siyuanshi.result);
				}
				else if (str_2 == "0") {
					tofmips("    bne   " + str_1 + ",    $0,    " + siyuanshi.result);
				}
				else {
					tofmips("    bne   " + str_1 + ",    " + str_2 + ",    " + siyuanshi.result);
				}
			}
			clear_t();
		}
		else if (siyuanshi.op == "!=") {
			str_1 = order_lw(siyuanshi.leftnum, siyuanshi.left_type);
			str_2 = order_lw(siyuanshi.rightnum, siyuanshi.right_type);
			tomiddle(siyuanshi_list.back().output_string());
			siyuanshi_list.pop_back();
			siyuanshi = siyuanshi_list.back();
			if (siyuanshi.op == "BZ_jump") {
				tofmips("    bne   " + str_1 + ",    " + str_2 + ",    " + siyuanshi.result);
			}
			else {
				assert(siyuanshi.op == "BNZ_jump");
				tofmips("    beq   " + str_1 + ",    " + str_2 + ",    " + siyuanshi.result);
			}
			clear_t();
		}
		else if (siyuanshi.op == ">=") {
			str_3 = find_reg(siyuanshi.result, siyuanshi.result_type);
			if (siyuanshi.left_type[0] == '5' && siyuanshi.right_type[0] == '5') {
				int left = stoi(siyuanshi.leftnum);
				int right = stoi(siyuanshi.rightnum);
				// 用addi少一条指令,所以左右倒过来
				tofmips("    addi  " + str_3 + ",    $0,    " + to_string(right - left));
			}
			else if (siyuanshi.right_type[0] == '5') {
				int right = stoi(siyuanshi.rightnum);
				str_1 = order_lw(siyuanshi.leftnum, siyuanshi.left_type, "");
				if (right >= 0) {
					tofmips("    addi  " + str_3 + ",    " + str_1 + ",    -" + to_string(right));
				}
				else {
					right = -right;
					tofmips("    addi  " + str_3 + ",    " + str_1 + ",    " + to_string(right));
				}
			}
			else {
				str_1 = order_lw(siyuanshi.leftnum, siyuanshi.left_type);
				str_2 = order_lw(siyuanshi.rightnum, siyuanshi.right_type);
				tofmips("    sub   " + str_3 + ",    " + str_1 + ",    " + str_2);
			}
			tomiddle(siyuanshi_list.back().output_string());
			siyuanshi_list.pop_back();
			siyuanshi = siyuanshi_list.back();
			if (siyuanshi.op == "BZ_jump") {
				tofmips("    bgez  " + str_3 + ",    " + siyuanshi.result);
			}
			else {
				assert(siyuanshi.op == "BNZ_jump");
				tofmips("    bltz  " + str_3 + ",    " + siyuanshi.result);
			}
			clear_t();
		}
		else if (siyuanshi.op == ">") {
			str_3 = find_reg(siyuanshi.result, siyuanshi.result_type);
			if (siyuanshi.left_type[0] == '5' && siyuanshi.right_type[0] == '5') {
				int left = stoi(siyuanshi.leftnum);
				int right = stoi(siyuanshi.rightnum);
				// 用addi少一条指令,所以左右倒过来
				tofmips("    addi  " + str_3 + ",    $0,    " + to_string(right - left));
			}
			else if (siyuanshi.right_type[0] == '5') {
				int right = stoi(siyuanshi.rightnum);
				str_1 = order_lw(siyuanshi.leftnum, siyuanshi.left_type, "");
				if (right >= 0) {
					tofmips("    addi  " + str_3 + ",    " + str_1 + ",    -" + to_string(right));
				}
				else {
					right = -right;
					tofmips("    addi  " + str_3 + ",    " + str_1 + ",    " + to_string(right));
				}
			}
			else {
				str_1 = order_lw(siyuanshi.leftnum, siyuanshi.left_type);
				str_2 = order_lw(siyuanshi.rightnum, siyuanshi.right_type);
				tofmips("    sub   " + str_3 + ",    " + str_1 + ",    " + str_2);
			}
			tomiddle(siyuanshi_list.back().output_string());
			siyuanshi_list.pop_back();
			siyuanshi = siyuanshi_list.back();
			if (siyuanshi.op == "BZ_jump") {
				tofmips("    bgtz  " + str_3 + ",    " + siyuanshi.result);
			}
			else {
				assert(siyuanshi.op == "BNZ_jump");
				tofmips("    blez  " + str_3 + ",    " + siyuanshi.result);
			}
			clear_t();
		}
		else if (siyuanshi.op == "<=") {
			str_3 = find_reg(siyuanshi.result, siyuanshi.result_type);
			if (siyuanshi.left_type[0] == '5' && siyuanshi.right_type[0] == '5') {
				int left = stoi(siyuanshi.leftnum);
				int right = stoi(siyuanshi.rightnum);
				// 用addi少一条指令,所以左右倒过来
				tofmips("    addi  " + str_3 + ",    $0,    " + to_string(right - left));
			}
			else if (siyuanshi.right_type[0] == '5') {
				int right = stoi(siyuanshi.rightnum);
				str_1 = order_lw(siyuanshi.leftnum, siyuanshi.left_type, "");
				if (right >= 0) {
					tofmips("    addi  " + str_3 + ",    " + str_1 + ",    -" + to_string(right));
				}
				else {
					right = -right;
					tofmips("    addi  " + str_3 + ",    " + str_1 + ",    " + to_string(right));
				}
			}
			else {
				str_1 = order_lw(siyuanshi.leftnum, siyuanshi.left_type);
				str_2 = order_lw(siyuanshi.rightnum, siyuanshi.right_type);
				tofmips("    sub   " + str_3 + ",    " + str_1 + ",    " + str_2);
			}
			tomiddle(siyuanshi_list.back().output_string());
			siyuanshi_list.pop_back();
			siyuanshi = siyuanshi_list.back();
			if (siyuanshi.op == "BZ_jump") {
				tofmips("    blez  " + str_3 + ",    " + siyuanshi.result);
			}
			else {
				assert(siyuanshi.op == "BNZ_jump");
				tofmips("    bgtz  " + str_3 + ",    " + siyuanshi.result);
			}
			clear_t();
		}
		else if (siyuanshi.op == "<") {
			str_3 = find_reg(siyuanshi.result, siyuanshi.result_type);
			if (siyuanshi.left_type[0] == '5' && siyuanshi.right_type[0] == '5') {
				int left = stoi(siyuanshi.leftnum);
				int right = stoi(siyuanshi.rightnum);
				// 用addi少一条指令,所以左右倒过来
				tofmips("    addi  " + str_3 + ",    $0,    " + to_string(right - left));
			}
			else if (siyuanshi.right_type[0] == '5') {
				int right = stoi(siyuanshi.rightnum);
				str_1 = order_lw(siyuanshi.leftnum, siyuanshi.left_type, "");
				if (right >= 0) {
					tofmips("    addi  " + str_3 + ",    " + str_1 + ",    -" + to_string(right));
				}
				else {
					right = -right;
					tofmips("    addi  " + str_3 + ",    " + str_1 + ",    " + to_string(right));
				}
			}
			else {
				str_1 = order_lw(siyuanshi.leftnum, siyuanshi.left_type);
				str_2 = order_lw(siyuanshi.rightnum, siyuanshi.right_type);
				tofmips("    sub   " + str_3 + ",    " + str_1 + ",    " + str_2);
			}
			tomiddle(siyuanshi_list.back().output_string());
			siyuanshi_list.pop_back();
			siyuanshi = siyuanshi_list.back();
			if (siyuanshi.op == "BZ_jump") {
				tofmips("    bltz  " + str_3 + ",    " + siyuanshi.result);
			}
			else  {
				assert(siyuanshi.op == "BNZ_jump");
				tofmips("    bgez  " + str_3 + ",    " + siyuanshi.result);
			}
			clear_t();
		}
		else if (siyuanshi.op == "BZ_jump") {
			str_1 = order_lw(siyuanshi.leftnum, siyuanshi.left_type);
			tofmips("    bne   " + str_1 + ",    $0,    " + siyuanshi.result);
			clear_t();
		}
		else if (siyuanshi.op == "BNZ_jump") {
			str_1 = order_lw(siyuanshi.leftnum, siyuanshi.left_type);
			tofmips("    beq   " + str_1 + ",    $0,    " + siyuanshi.result);
			clear_t();
		}

		else if (siyuanshi.op == "jump") {
			// 只能跳转256MB 区域(26位)
			tofmips("    j    " + siyuanshi.result);
			clear_t();
		}
		else if (siyuanshi.op == "set_label") {
			tofmips(siyuanshi.result + ":");
			clear_t();
		}
		else if (siyuanshi.op == "inline_clear") {
			while (inline_regs.size() > 0) {
				refresh_reg(inline_regs.back(), "8", 1);
				inline_regs.pop_back();
				continue;
			}
		}
		else if (siyuanshi.op == "hanshu") {
			tofmips("");
			tomiddle("");
			tofmips(siyuanshi.result + ":");
			// 刷新一下
			clear_t();
			clear_s();
			local_name_offset.clear();
			offset_end = 0;
		}
		else if (siyuanshi.op == "call_start") {
			//  储存起来 t_regs 栈顶的元素
			if (t_reg_addr_map[t_regs.front()] != "1") {
				if (DEBUG_MIPS == 1) {
					cout << "call_start: " << t_regs.front() << " t_reg_addr_map[t_regs.front()]: " << t_reg_addr_map[t_regs.front()] << endl;
				}
				tofmips(" sw    " + t_regs.front() + ",    " + t_reg_addr_map[t_regs.front()] + "($sp)");
			}
			if (push_index.size() > 0 && push_index.front() > 0) {
				//tofmips("    subi  $gp,    $gp,    " + to_string(push_index.front() * 4));
			}
			push_index.push_front(0);
		}
		else if (siyuanshi.op == "push") {
			str_1 = order_lw(siyuanshi.result, siyuanshi.result_type);
			// tofmips("    sw    " + str_1 + ",    -" + to_string(push_index.front() * 4) + "($gp)");
			tofmips("    sw    " + str_1 + ",    ($gp)");
			tofmips("    addi  $gp,    $gp,    -4");
			push_index.front()++;
		}
		else if (siyuanshi.op == "call_end") {
			save_environment();
			//tofmips("    subi  $gp,    $gp,    " + to_string(push_index.front() * 4));
			tofmips("");
			tofmips("    jal    " + siyuanshi.result);
			tofmips("");
			recover_environment();
			push_index.pop_front();
			clear_t();
		}
		else if (siyuanshi.op == "ret") {
			tofmips("    jr    $31");
		}
		else if (siyuanshi.op == "ret()") {
			order_lw(siyuanshi.result, siyuanshi.result_type, "$v1");
			tofmips("    jr    $31");
		}
		else if (siyuanshi.op == "[]load") {
			str_2 = t_regs_89.back();
			t_regs_89.pop_back();
			t_regs_89.push_front(str_2);
			str_3 = find_reg(siyuanshi.result, siyuanshi.result_type);
			if (siyuanshi.right_type[0] == '5') {
				int offset = stoi(siyuanshi.rightnum);
				offset *= 4;
				if (siyuanshi.left_type[2] == '0') {
					if (offset >= 0) {
						tofmips("    addi  " + str_2 + ",    $k1,    -" + to_string(offset));
					}
					else {
						tofmips("    addi  " + str_2 + ",    $k1,    " + to_string(-offset));
					}
					offset = stoi(global_name_offset[siyuanshi.leftnum]);
				}
				else {
					if (offset >= 0) {
						tofmips("    addi  " + str_2 + ",    $sp,    -" + to_string(offset));
					}
					else {
						tofmips("    addi  " + str_2 + ",    $sp,    " + to_string(-offset));
					}
					offset = stoi(local_name_offset[siyuanshi.leftnum]);
				}
				tofmips("    lw    " + str_3 + ",    " + to_string(offset) + "(" + str_2 + ")");
			}
			else {
				str_1 = order_lw(siyuanshi.rightnum, siyuanshi.right_type);
				tofmips("    sll   " + str_2 + ",    " + str_1 + ",    2");
				int offset;
				if (siyuanshi.left_type[2] == '0') {
					offset = stoi(global_name_offset[siyuanshi.leftnum]);
					tofmips("    sub   " + str_2 + ",     $k1,    " + str_2);
				}
				else {
					offset = stoi(local_name_offset[siyuanshi.leftnum]);
					tofmips("    sub   " + str_2 + ",    $sp,    " + str_2);
				}
				tofmips("    lw    " + str_3 + ",    " + to_string(offset) + "(" + str_2 + ")");
			}
		}
		else if (siyuanshi.op == "[]store") {
			str_1 = order_lw(siyuanshi.rightnum, siyuanshi.right_type);
			str_3 = t_regs_89.back();
			t_regs_89.pop_back();
			t_regs_89.push_front(str_3);
			if (siyuanshi.left_type[0] == '5') {
				int offset = stoi(siyuanshi.leftnum);
				offset *= 4;
				if (siyuanshi.result_type[2] == '0') {
					if (offset >= 0) {
						tofmips("    addi  " + str_3 + ",    $k1,    -" + to_string(offset));
					}
					else {
						tofmips("    addi  " + str_3 + ",    $k1,    " + to_string(-offset));
					}
					offset = stoi(global_name_offset[siyuanshi.result]);
				}
				else {
					if (offset >= 0) {
						tofmips("    addi  " + str_3 + ",    $sp,    -" + to_string(offset));
					}
					else {
						tofmips("    addi  " + str_3 + ",    $sp,    " + to_string(-offset));
					}
					offset = stoi(local_name_offset[siyuanshi.result]);
				}
				tofmips("    sw    " + str_1 + ",    " + to_string(offset) + "(" + str_3 + ")");
			}
			else {
				str_2 = order_lw(siyuanshi.leftnum, siyuanshi.left_type);
				tofmips("    sll   " + str_3 + ",    " + str_2 + ",    2");
				int offset;
				if (siyuanshi.result_type[2] == '0') {
					offset = stoi(global_name_offset[siyuanshi.result]);
					tofmips("    sub   " + str_3 + ",    $k1,    " + str_3);
				}
				else {
					offset = stoi(local_name_offset[siyuanshi.result]);
					tofmips("    sub   " + str_3 + ",    $sp,    " + str_3);
				}
				tofmips("    sw    " + str_1 + ",    " + to_string(offset) + "(" + str_3 + ")");
			}
		}
		else if (siyuanshi.op == "var") {
			local_name_offset[siyuanshi.result] = to_string(offset_end);
			offset_end -= 4;
		}
		else if (siyuanshi.op == "var[]") {
			local_name_offset[siyuanshi.result] = to_string(offset_end);
			offset_end -= 4 * stoi(siyuanshi.leftnum);
		}
		else if (siyuanshi.op == "para") {
			assert(offset_end == 0);
			local_name_offset[siyuanshi.result] = to_string(offset_end);
			offset_end -= 4;
			list<Siyuanshi> list_tmp;
			list_tmp.push_back(siyuanshi);
			tomiddle(siyuanshi_list.back().output_string());
			siyuanshi_list.pop_back();
			while (siyuanshi_list.back().op == "para") {
				local_name_offset[siyuanshi_list.back().result] = to_string(offset_end);
				offset_end -= 4;
				siyuanshi = siyuanshi_list.back();
				list_tmp.push_back(siyuanshi);
				tomiddle(siyuanshi_list.back().output_string());
				siyuanshi_list.pop_back();
			}
			while (list_tmp.size() > 0) {
				// 逆序取出(栈的性质)
				str_1 = find_reg(list_tmp.back().result, list_tmp.back().result_type);
				tofmips("    addi  $gp,    $gp,    4");
				tofmips("    lw    " + str_1 + ",    0($gp)");
				list_tmp.pop_back();
			}
			continue;
		}
		else if (siyuanshi.op == "scanf") {
			if (siyuanshi.result_type[0] == '1' || siyuanshi.result_type[0] == '3') {
				str = "    li    $v0,    5";
			}
			else if (siyuanshi.result_type[0] == '2' || siyuanshi.result_type[0] == '4' 
				|| (siyuanshi.result_type.size() >= 2 && siyuanshi.result_type[1] == '1')) {
				str = "    li    $v0,    12";
			}
			tofmips(str);
			tofmips("    syscall");
			order_sw(siyuanshi, "$v0");
		}
		else if (siyuanshi.op == "print" || siyuanshi.op == "print2") {
			if (siyuanshi.op == "print2") {
				tofmips("    la    $a0,    " + siyuanshi.leftnum);
				tofmips("    li    $v0,    4");
				tofmips("    syscall");
			}
			if (siyuanshi.result_type == "7") {
				tofmips("    la    $a0,    " + siyuanshi.result);
				str = "    li    $v0,    4";
			}
			else if (siyuanshi.result_type == "9") {
				tofmips("    move  $a0,    $v1");
				str = "    li    $v0,    1";
			}
			else if (siyuanshi.result_type == "10") {  // 两位数放前面识别,放在识别到下一个里面去
				tofmips("    move  $a0,    $v1");
				str = "    li    $v0,    11";
			}
			else {
				str_1 = order_lw(siyuanshi.result, siyuanshi.result_type, "$a0");
				if (siyuanshi.result_type.size() >= 2 && siyuanshi.result_type[1] == '1') {
					str = "    li    $v0,    11";
				}
				else if (siyuanshi.result_type[0] == '1' || siyuanshi.result_type[0] == '3'
					|| siyuanshi.result_type == "5" || siyuanshi.result_type[0] == '8') {
					str = "    li    $v0,    1";
				}
				else if (siyuanshi.result_type[0] == '2' || siyuanshi.result_type[0] == '4'
					|| siyuanshi.result_type == "6") {
					str = "    li    $v0,    11";
				}
			}
			tofmips(str);
			tofmips("    syscall");
			tofmips("    lui   $a0,    0x1001");
			tofmips("    li    $v0,    4");
			tofmips("    syscall");
			refresh_reg(siyuanshi.result, siyuanshi.result_type);
		}
		refresh_reg(siyuanshi.leftnum, siyuanshi.left_type);
		refresh_reg(siyuanshi.rightnum, siyuanshi.right_type);
		tomiddle(siyuanshi_list.back().output_string());
		siyuanshi_list.pop_back();
	}
	tofmips("");
	tofmips("$END:");
	// tofmips("    li    $v0,    10");
	// tofmips("    syscall");
	return;
}