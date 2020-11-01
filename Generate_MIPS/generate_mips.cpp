#include "my.h"

map<string, string> local_name_offset;
map<string, string> global_name_offset;
list<int> push_index;
int offset_end = 0;
int offset_global_array = 268697596;  // 0x10040000 - 0x4

// type 0: left, 1: right 
string order_lw(string source, string type, string aim) {
	string str;
	if (DEBUG == 1) {
		cout << "order_lw source: " << source << " type: " << type << endl;
	}
	if (type[0] == '5') {
		str = "li " + aim + ", " + source;
	}
	else if (type[0] == '6') {
		str = "li " + aim + ", '" + source + "'";
	}
	else if (type == "9") {
		str = "add " + aim + ", $0, $a1";
	}
	else if (type == "10") {
		str = "add " + aim + ", $0, $a1";
	}
	else if (type.length() == 3 && type[2] == '0') {
		str = "lw " + aim + ", " + source;
	}
	else {
		str = "lw " + aim + ", " + local_name_offset[source] + "($sp)";
	}
	tofmips(str);
	return str;
}

string order_sw(Siyuanshi siyuanshi, string aim) {
	string str;
	if (DEBUG == 1) {
		cout << "order_sw: " << siyuanshi.result_type << " count: " << local_name_offset.count(siyuanshi.result) << endl;
	}
	if (siyuanshi.result_type[0] == '8' && local_name_offset.count(siyuanshi.result) == 0) {
		local_name_offset[siyuanshi.result] = to_string(offset_end);
		offset_end -= 4;
	}
	if (siyuanshi.result_type.length() == 3 && siyuanshi.result_type[2] == '0') {
		str = "sw " + aim + ", " + siyuanshi.result;
	}
	else {
		str = "sw " + aim + ", " + local_name_offset[siyuanshi.result] + "($sp)";
	}
	tofmips(str);
	return str;
}

void save_environment() {
	tofmips("addi $sp, $sp, " + to_string(offset_end));
	tofmips("sw $ra, 0($sp)");
	tofmips("addi $sp, $sp, -4");
	tofmips("sw $fp, 0($sp)");
	tofmips("addi $sp, $sp, -4");
	tofmips("add $fp, $0, $sp");
	// TODO: 以后要加入寄存器的保存
}

void recover_environment() {
	// TODO: 以后要加入寄存器的恢复
	tofmips("add $sp, $0, $fp");
	tofmips("subi $sp, $sp, -4");
	tofmips("lw $fp, 0($sp)");
	tofmips("subi $sp, $sp, -4");
	tofmips("lw $ra, 0($sp)");
	tofmips("subi $sp, $sp, " + to_string(offset_end));
}

void generate_mips() {
	tomiddle(".data");
	tofmips(".data");
	tofmips("enter: .asciiz \"\\n\"");
	while (siyuanshi_data_list.size() > 0) { 
		Siyuanshi siyuanshi = siyuanshi_data_list.back();
		if (siyuanshi.op == ".asciiz") {
			string str = siyuanshi.result + ": " + siyuanshi.op + " " + siyuanshi.leftnum;
			tofmips(str);
		}
		siyuanshi_data_list.pop_back();
	}
	while (1) {
		Siyuanshi siyuanshi = siyuanshi_list.back();
		if (siyuanshi.op == "hanshu") {
			break;
		}
		else if (siyuanshi.op == "var" && siyuanshi.result_type.length() == 3 && siyuanshi.result_type[2] == '0') {
			string str = siyuanshi.result + ": .word ";
			if (siyuanshi.result_type[0] == '1') {
				tomiddle(siyuanshi_list.back().output_string());
				siyuanshi_list.pop_back();
				siyuanshi = siyuanshi_list.back();
				if (siyuanshi.op == "--") {
					string num = siyuanshi.leftnum;
					tomiddle(siyuanshi_list.back().output_string());
					siyuanshi_list.pop_back();
					siyuanshi = siyuanshi_list.back();
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
				str += "'" + siyuanshi.leftnum + "'";
			}
			else {
				str += "0";
			}
			tofmips(str);
		}
		else if (siyuanshi.op == "var[]" && siyuanshi.result_type.length() == 3 && siyuanshi.result_type[2] == '0') {
			global_name_offset[siyuanshi.result] = to_string(offset_global_array);
			offset_global_array -= 4 * stoi(siyuanshi.leftnum);
			if (DEBUG == 1) {
				cout << "offset_global_array[]: " << offset_global_array << endl;
			}
		}
		else {
			cout << "!!!!!!!!!!!!!!!!!!!!!!! op wrong: " << siyuanshi.op << endl;
		}
		tomiddle(siyuanshi_list.back().output_string());
		siyuanshi_list.pop_back();
	}
	tomiddle("");
	tomiddle(".text");
	tofmips("");
	tofmips(".text");
	tofmips("jal main");
	tofmips("j $END");
	while (siyuanshi_list.size() > 0) {
		Siyuanshi siyuanshi = siyuanshi_list.back();
		string str;
		if (siyuanshi.op == "+") {
			str = order_lw(siyuanshi.leftnum, siyuanshi.left_type, "$t0");
			str = order_lw(siyuanshi.rightnum, siyuanshi.right_type, "$t1");
			tofmips("add $t2, $t0, $t1");
			str = order_sw(siyuanshi, "$t2");
		}
		else if (siyuanshi.op == "-") {
			str = order_lw(siyuanshi.leftnum, siyuanshi.left_type, "$t0");
			str = order_lw(siyuanshi.rightnum, siyuanshi.right_type, "$t1");
			tofmips("sub $t2, $t0, $t1");
			str = order_sw(siyuanshi, "$t2");
		}
		else if (siyuanshi.op == "--") {
			str = order_lw(siyuanshi.leftnum, siyuanshi.left_type, "$t0");
			tofmips("sub $t2, $0, $t0");
			str = order_sw(siyuanshi, "$t2");
		}
		else if (siyuanshi.op == "*") {
			str = order_lw(siyuanshi.leftnum, siyuanshi.left_type, "$t0");
			str = order_lw(siyuanshi.rightnum, siyuanshi.right_type, "$t1");
			tofmips("mult $t0, $t1");
			// result 也是int,所以不用考虑HI寄存器(高32位)
			tofmips("mflo $t2");
			str = order_sw(siyuanshi, "$t2");
		}
		else if (siyuanshi.op == "/") {
			str = order_lw(siyuanshi.leftnum, siyuanshi.left_type, "$t0");
			str = order_lw(siyuanshi.rightnum, siyuanshi.right_type, "$t1");
			tofmips("div $t0, $t1");
			// result 也是int,所以不用考虑HI寄存器(余数)
			tofmips("mflo $t2");
			str = order_sw(siyuanshi, "$t2");
		}
		else if (siyuanshi.op == "=") {
			str = order_lw(siyuanshi.leftnum, siyuanshi.left_type, "$t0");
			tofmips("add $t2, $0, $t0");
			str = order_sw(siyuanshi, "$t2");
		}
		else if (siyuanshi.op == "==") {
			str = order_lw(siyuanshi.leftnum, siyuanshi.left_type, "$t0");
			str = order_lw(siyuanshi.rightnum, siyuanshi.right_type, "$t1");
			tomiddle(siyuanshi_list.back().output_string());
			siyuanshi_list.pop_back();
			siyuanshi = siyuanshi_list.back();
			if (siyuanshi.op == "BZ_jump") {
				tofmips("beq $t0, $t1, " + siyuanshi.result);
			}
			else if (siyuanshi.op == "BNZ_jump") {
				tofmips("bne $t0, $t1, " + siyuanshi.result);
			}
		}
		else if (siyuanshi.op == "!=") {
			str = order_lw(siyuanshi.leftnum, siyuanshi.left_type, "$t0");
			str = order_lw(siyuanshi.rightnum, siyuanshi.right_type, "$t1");
			tomiddle(siyuanshi_list.back().output_string());
			siyuanshi_list.pop_back();
			siyuanshi = siyuanshi_list.back();
			if (siyuanshi.op == "BZ_jump") {
				tofmips("bne $t0, $t1, " + siyuanshi.result);
			}
			else if (siyuanshi.op == "BNZ_jump") {
				tofmips("beq $t0, $t1, " + siyuanshi.result);
			}
		}
		else if (siyuanshi.op == ">=") {
			str = order_lw(siyuanshi.leftnum, siyuanshi.left_type, "$t0");
			str = order_lw(siyuanshi.rightnum, siyuanshi.right_type, "$t1");
			tofmips("sub $t2, $t0, $t1");
			str = order_sw(siyuanshi, "$t2");
			tomiddle(siyuanshi_list.back().output_string());
			siyuanshi_list.pop_back();
			siyuanshi = siyuanshi_list.back();
			if (siyuanshi.op == "BZ_jump") {
				tofmips("bgez, $t2, " + siyuanshi.result);
			}
			else if (siyuanshi.op == "BNZ_jump") {
				tofmips("bltz, $t2, " + siyuanshi.result);
			}
		}
		else if (siyuanshi.op == ">") {
			str = order_lw(siyuanshi.leftnum, siyuanshi.left_type, "$t0");
			str = order_lw(siyuanshi.rightnum, siyuanshi.right_type, "$t1");
			tofmips("sub $t2, $t0, $t1");
			str = order_sw(siyuanshi, "$t2");
			tomiddle(siyuanshi_list.back().output_string());
			siyuanshi_list.pop_back();
			siyuanshi = siyuanshi_list.back();
			if (siyuanshi.op == "BZ_jump") {
				tofmips("bgtz, $t2, " + siyuanshi.result);
			}
			else if (siyuanshi.op == "BNZ_jump") {
				tofmips("blez, $t2, " + siyuanshi.result);
			}
		}
		else if (siyuanshi.op == "<=") {
			str = order_lw(siyuanshi.leftnum, siyuanshi.left_type, "$t0");
			str = order_lw(siyuanshi.rightnum, siyuanshi.right_type, "$t1");
			tofmips("sub $t2, $t0, $t1");
			str = order_sw(siyuanshi, "$t2");
			tomiddle(siyuanshi_list.back().output_string());
			siyuanshi_list.pop_back();
			siyuanshi = siyuanshi_list.back();
			if (siyuanshi.op == "BZ_jump") {
				tofmips("blez, $t2, " + siyuanshi.result);
			}
			else if (siyuanshi.op == "BNZ_jump") {
				tofmips("bgtz, $t2, " + siyuanshi.result);
			}
		}
		else if (siyuanshi.op == "<") {
			str = order_lw(siyuanshi.leftnum, siyuanshi.left_type, "$t0");
			str = order_lw(siyuanshi.rightnum, siyuanshi.right_type, "$t1");
			tofmips("sub $t2, $t0, $t1");
			str = order_sw(siyuanshi, "$t2");
			tomiddle(siyuanshi_list.back().output_string());
			siyuanshi_list.pop_back();
			siyuanshi = siyuanshi_list.back();
			if (siyuanshi.op == "BZ_jump") {
				tofmips("bltz, $t2, " + siyuanshi.result);
			}
			else if (siyuanshi.op == "BNZ_jump") {
				tofmips("bgez, $t2, " + siyuanshi.result);
			}
		}
		else if (siyuanshi.op == "BZ_jump") {
			str = order_lw(siyuanshi.leftnum, siyuanshi.left_type, "$t0");
			tofmips("bne $t0, $0, " + siyuanshi.result);
		}
		else if (siyuanshi.op == "BNZ_jump") {
			str = order_lw(siyuanshi.leftnum, siyuanshi.left_type, "$t0");
			tofmips("beq $t0, $0, " + siyuanshi.result);
		}
		
		else if (siyuanshi.op == "jump") {
		// 只能跳转256MB 区域(26位)
			tofmips("j " + siyuanshi.result);
		}
		else if (siyuanshi.op == "set_label") {
			tofmips(siyuanshi.result + ":");
		}
		else if (siyuanshi.op == "hanshu") {
			tofmips("");
			tomiddle("");
			tofmips(siyuanshi.result + ":");
			// 刷新一下
			local_name_offset.clear();
			offset_end = 0;
		}
		else if (siyuanshi.op == "call_start") {
			push_index.push_front(0);
		}
		else if (siyuanshi.op == "push") {
			/*
			if (push_index.front() == 0) {
				order_lw(siyuanshi.result, siyuanshi.result_type, "$a0");
			}
			else if (push_index.front() == 1) {
				order_lw(siyuanshi.result, siyuanshi.result_type, "$a1");
			}
			else if (push_index.front() == 2) {
				order_lw(siyuanshi.result, siyuanshi.result_type, "$a2");
			}
			else if (push_index.front() == 3) {
				order_lw(siyuanshi.result, siyuanshi.result_type, "$a3");
			}
			else { 
				*/
				order_lw(siyuanshi.result, siyuanshi.result_type, "$t0");
				tofmips("sw $t0, 0($gp)");
				tofmips("subi $gp, $gp, 4");
			//}
			push_index.front()++;
		}
		else if (siyuanshi.op == "call_end") {
			save_environment();
			tofmips("");
			tofmips("jal " + siyuanshi.result);
			tofmips("");
			recover_environment();
			push_index.pop_front();
		}
		else if (siyuanshi.op == "ret") {
			tofmips("jr $31");
		}
		else if (siyuanshi.op == "ret()") {
			str = order_lw(siyuanshi.result, siyuanshi.result_type, "$a1");
			tofmips("jr $31");
		}
		else if (siyuanshi.op == "[]load") {
			/*if (siyuanshi.right_type == "5") {
				int offset = stoi(global_name_offset[siyuanshi.leftnum]) - 4 * stoi(siyuanshi.rightnum);
				if (siyuanshi.left_type[2] == '0') {
					tofmips("lw $t2, " + to_string(offset) + "($0)");
				}
				else {
					tofmips("lw $t2, " + to_string(offset) + "($sp)");
				}
			}*/
			if (1){
				str = order_lw(siyuanshi.rightnum, siyuanshi.right_type, "$t0");
				tofmips("sll $t0, $t0, 2");
				int offset;
				if (siyuanshi.left_type[2] == '0') {
					offset = stoi(global_name_offset[siyuanshi.leftnum]);
					tofmips("sub $t0, $0, $t0");
				}
				else {
					offset = stoi(local_name_offset[siyuanshi.leftnum]);
					tofmips("sub $t0, $sp, $t0");
				}
				tofmips("lw $t2, " + to_string(offset) + "($t0)");
			}
			str = order_sw(siyuanshi, "$t2");
		}
		else if (siyuanshi.op == "[]store") {
			str = order_lw(siyuanshi.rightnum, siyuanshi.right_type, "$t0");
			str = order_lw(siyuanshi.leftnum, siyuanshi.left_type, "$t1");
			tofmips("sll $t1, $t1, 2");
			int offset;
			if (siyuanshi.result_type[2] == '0') {
				offset = stoi(global_name_offset[siyuanshi.result]);
				tofmips("sub $t1, $0, $t1");
			}
			else {
				offset = stoi(local_name_offset[siyuanshi.result]);
				tofmips("sub $t1, $sp, $t1");
			}
			tofmips("sw $t0, " + to_string(offset) + "($t1)");
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
				tofmips("addi $gp, $gp, 4"); 
				tofmips("lw $t0, 0($gp)");
				order_sw(list_tmp.back(), "$t0");  // 逆序取出(栈的性质)
				list_tmp.pop_back();
			}
			continue;
		}
		else if (siyuanshi.op == "scanf") {
			if (siyuanshi.result_type[0] == '1' || siyuanshi.result_type[0] == '3') {
				str = "li $v0, 5";
			}
			else if (siyuanshi.result_type[0] == '2' || siyuanshi.result_type[0] == '4') {
				str = "li $v0, 12";
			}
			tofmips(str);
			tofmips("syscall");
			order_sw(siyuanshi, "$v0");
		}
		else if (siyuanshi.op == "print" || siyuanshi.op == "print2") {
			if (siyuanshi.op == "print2") {
				tofmips("la $a0, " + siyuanshi.leftnum);
				tofmips("li $v0, 4");
				tofmips("syscall");
			}
			if (siyuanshi.result_type == "7") {
				tofmips("la $a0, " + siyuanshi.result);
				str = "li $v0, 4";
			}
			else if (siyuanshi.result_type == "9") {
				tofmips("add $a0, $0, $a1");
				str = "li $v0, 1";
			}
			else if (siyuanshi.result_type == "10") {  // 两位数放前面识别,放在识别到下一个里面去
				tofmips("add $a0, $0, $a1");
				str = "li $v0, 11";
			}
			else {
				order_lw(siyuanshi.result, siyuanshi.result_type, "$a0");
				if (siyuanshi.result_type.size() >= 2 && siyuanshi.result_type[1] == '1') {
					str = "li $v0, 11";
				}
				else if (siyuanshi.result_type[0] == '1' || siyuanshi.result_type[0] == '3' 
					|| siyuanshi.result_type == "5" || siyuanshi.result_type[0] == '8') {
					str = "li $v0, 1";
				}
				else if (siyuanshi.result_type[0] == '2' || siyuanshi.result_type[0] == '4'
					|| siyuanshi.result_type == "6") {
					str = "li $v0, 11";
				}
			}
			tofmips(str);
			tofmips("syscall");
			tofmips("la $a0, enter");
			tofmips("li $v0, 4");
			tofmips("syscall");
		}
		tomiddle(siyuanshi_list.back().output_string());
		siyuanshi_list.pop_back();
	}
	tofmips("");
	tofmips("$END:");
	tofmips("li $v0, 10");
	tofmips("syscall");
	return;
}