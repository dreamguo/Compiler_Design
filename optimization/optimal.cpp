#include "my.h"

list<Siyuanshi> tmp_siyuanshi_list;
map<string, list<Siyuanshi>> name_inline_map;
list<string> inline_reg_list;
int inline_reg_start = 0;
set<string> tmp_test;

void reg_optimal() {
	while (siyuanshi_list.size() > 0) {
		Siyuanshi siyuanshi = siyuanshi_list.back();
		siyuanshi_list.pop_back();
		if ((siyuanshi.op == "+" || siyuanshi.op == "-" || siyuanshi.op == "*" ||
			siyuanshi.op == "/" || siyuanshi.op == "=" ||  siyuanshi.op == "--")
			&& siyuanshi.result[0] == '$' && siyuanshi.result[1] == 'r') {
			Siyuanshi siyuanshi_2 = siyuanshi_list.back();
			if (siyuanshi_2.op == "=" && siyuanshi_2.leftnum == siyuanshi.result) {
				assert(siyuanshi.result_type[0] != '3');
				// assert(siyuanshi_2.result_type[0] != '3');
				if (siyuanshi_2.result_type[0] == '4' || siyuanshi_2.result_type[0] == '8') {
					tmp_test.insert(siyuanshi.result);
					siyuanshi.result = siyuanshi_2.result;
					siyuanshi.result_type = siyuanshi_2.result_type;
					siyuanshi_list.pop_back();
				}
			}
		}
		assert(tmp_test.count(siyuanshi.result) == 0);
		assert(tmp_test.count(siyuanshi.rightnum) == 0);
		assert(tmp_test.count(siyuanshi.leftnum) == 0);
		tmp_siyuanshi_list.push_front(siyuanshi);
	}
}

void inline_init() {
	for (list<Siyuanshi>::reverse_iterator iter = tmp_siyuanshi_list.rbegin(); iter != tmp_siyuanshi_list.rend(); ) {
		if (iter->op == "hanshu" && iter->result != "main") {
			list<Siyuanshi> hanshu_list;
			string name = iter->result;
			int is_inline = 1;
			for (; iter != tmp_siyuanshi_list.rbegin(); ) {
				hanshu_list.push_front(*iter);
				iter++;
				if (iter->op == "call_start" || iter->op == "set_label" || iter->op == "jump" ||
					iter->op == "BZ_jump" || iter->op == "BNZ_jump" || iter->op == "var" ||
					iter->op == "var[]" || iter->op == "[]store" || iter->op == "[]load") {
					is_inline = 0;
				}
				if (iter->op == "hanshu") {
					break;
				}
			}
			if (is_inline == 1) {
				//assert(hanshu_list.size() < 20);  // TODO: 非常危险
				name_inline_map[name] = hanshu_list;
			}
		}
		else {
			iter++;
		}
	}
	if (DEBUG_INLINE == 1) {
		for (map<string, list<Siyuanshi>>::reverse_iterator iter = name_inline_map.rbegin(); iter != name_inline_map.rend(); iter++) {
			for (list<Siyuanshi>::reverse_iterator it = (iter->second).rbegin(); it != (iter->second).rend(); it++) {
				cout << it->op << "    " << it->output_string() << endl;
			}
			cout << endl;
		}
	}
}

void inline_optimal() {
	while (tmp_siyuanshi_list.size() > 0) {
		Siyuanshi siyuanshi = tmp_siyuanshi_list.back();
		tmp_siyuanshi_list.pop_back();
		if (siyuanshi.op == "call_start" && name_inline_map.count(siyuanshi.result) > 0) {
			if (inline_reg_list.size() < 10) {
				for (int i = inline_reg_start; i < inline_reg_start + 20; i++) {
					inline_reg_list.push_back("$inline_" + to_string(i));
				}
				inline_reg_start += 20;
			}
			list<Siyuanshi> tmp_diaoyong;
			while (1) {
				tmp_diaoyong.push_front(siyuanshi);
				siyuanshi = tmp_siyuanshi_list.back();
				tmp_siyuanshi_list.pop_back();
				if (siyuanshi.op == "call_start") {
					tmp_siyuanshi_list.push_back(siyuanshi);
					break;
				}
				else if (siyuanshi.op == "call_end") {
					tmp_diaoyong.push_front(siyuanshi);
					break;
				}
			}
			if (tmp_diaoyong.front().op != "call_end") {
				while (tmp_diaoyong.size() > 0) {
					siyuanshi = tmp_diaoyong.back();
					tmp_diaoyong.pop_back();
					siyuanshi_list.push_front(siyuanshi);
				}
				continue;
			}
			else {
				list<Siyuanshi> tmp_inline;
				copy(name_inline_map[tmp_diaoyong.back().result].begin(), name_inline_map[tmp_diaoyong.back().result].end(),
					back_inserter(tmp_inline));
				map<string, string> bianliang_inline;
				map<string, string> bianliang_inline_type;
				for (; tmp_diaoyong.size() > 0; tmp_diaoyong.pop_back()) {
					siyuanshi = tmp_diaoyong.back();
					if (siyuanshi.op == "call_start") {
						Siyuanshi siyuanshi_inline = tmp_inline.back();
						tmp_inline.pop_back();
						assert(siyuanshi.result == siyuanshi_inline.result);
					}
					else if (siyuanshi.op == "call_end") {
						;
					}
					else if (siyuanshi.op == "push") {
						Siyuanshi siyuanshi_inline = tmp_inline.back();
						tmp_inline.pop_back();
						if (siyuanshi.result[0] == '$') {
							string result = inline_reg_list.front();
							inline_reg_list.pop_front();
							if (siyuanshi_inline.result_type[0] == '3') {
								Siyuanshi siyuanshi_new("=", result, siyuanshi.result, NULL_CHAR,
									"8", siyuanshi.result_type, "0");
								siyuanshi_list.push_front(siyuanshi_new);
								bianliang_inline_type[siyuanshi_inline.result] = "8";
							}
							else if (siyuanshi_inline.result_type[0] == '4') {
								Siyuanshi siyuanshi_new("=", result, siyuanshi.result, NULL_CHAR,
									"81", siyuanshi.result_type, "0");
								siyuanshi_list.push_front(siyuanshi_new);
								bianliang_inline_type[siyuanshi_inline.result] = "81";
							}
							bianliang_inline[siyuanshi_inline.result] = result;
						}
						else {
							bianliang_inline_type[siyuanshi_inline.result] = siyuanshi.result_type;
							bianliang_inline[siyuanshi_inline.result] = siyuanshi.result;
						}
						if (DEBUG_INLINE == 1) {
							cout << siyuanshi_list.front().output_string() << endl;
						}
					}
					else {
						siyuanshi_list.push_front(siyuanshi);
						if (DEBUG_INLINE == 1) {
							cout << siyuanshi_list.front().output_string() << endl;
						}
					}
				}
				while (tmp_inline.size() > 0) {
					Siyuanshi siyuanshi_inline = tmp_inline.back();
					tmp_inline.pop_back();
					if (bianliang_inline.count(siyuanshi_inline.leftnum) > 0) {
						siyuanshi_inline.left_type = bianliang_inline_type[siyuanshi_inline.leftnum];
						siyuanshi_inline.leftnum = bianliang_inline[siyuanshi_inline.leftnum];
					}
					if (bianliang_inline.count(siyuanshi_inline.rightnum) > 0) {
						siyuanshi_inline.right_type = bianliang_inline_type[siyuanshi_inline.rightnum];
						siyuanshi_inline.rightnum = bianliang_inline[siyuanshi_inline.rightnum];
					}
					if (bianliang_inline.count(siyuanshi_inline.result) > 0) {
						if (siyuanshi_inline.result[0] != '$' && (siyuanshi_inline.op == "+" ||
							siyuanshi_inline.op == "-" || siyuanshi_inline.op == "*" ||
							siyuanshi_inline.op == "/" || siyuanshi_inline.op == "=" ||
							siyuanshi_inline.op == "--" || siyuanshi_inline.op == "scanf")) {
							string result = inline_reg_list.front();
							inline_reg_list.pop_front();
							if (siyuanshi_inline.result_type[0] == '2' || 
								siyuanshi_inline.result_type[0] == '4' || 
								siyuanshi_inline.result_type[0] == '6' || 
								siyuanshi_inline.result_type == "81") {
								bianliang_inline_type[siyuanshi_inline.result] = "81";
							}
							else {
								bianliang_inline_type[siyuanshi_inline.result] = "8";
							}
							bianliang_inline[siyuanshi_inline.result] = result;
						}
						siyuanshi_inline.result_type = bianliang_inline_type[siyuanshi_inline.result];
						siyuanshi_inline.result = bianliang_inline[siyuanshi_inline.result];
					}
					if (siyuanshi_inline.op == "ret()") {
						Siyuanshi tmp = tmp_siyuanshi_list.back();
						if (tmp.op == "=" && tmp.leftnum == "#ret_val") {
							tmp.leftnum = siyuanshi_inline.result;
							tmp.left_type = siyuanshi_inline.result_type;
							siyuanshi_list.push_front(tmp);
							if (DEBUG_INLINE == 1) {
								cout << siyuanshi_list.front().output_string() << endl;
							}
							tmp_siyuanshi_list.pop_back();
						}
						else {
							siyuanshi_inline.op = "=retval";  // 加入一种新的op
							siyuanshi_list.push_front(siyuanshi_inline);
							if (DEBUG_INLINE == 1) {
								cout << siyuanshi_list.front().output_string() << endl;
							}
						}
					}
					else if (siyuanshi_inline.op == "ret") {
						;
					}
					else {
						siyuanshi_list.push_front(siyuanshi_inline);
						if (DEBUG_INLINE == 1) {
							cout << siyuanshi_list.front().output_string() << endl;
						}
					}
				}
				Siyuanshi tmp("inline_clear", NULL_CHAR, NULL_CHAR, NULL_CHAR);
				siyuanshi_list.push_front(tmp);
			}
			if (DEBUG_INLINE == 1) {
				cout << endl;
			}
		}
		else {
			siyuanshi_list.push_front(siyuanshi);
		}
	}
}

void for_optimal() {
	list<map<string, list<Siyuanshi>>> for_siyuanshi_stack;
	while (siyuanshi_list.size() > 0) {
		Siyuanshi siyuanshi = siyuanshi_list.back();
		siyuanshi_list.pop_back();
		if (siyuanshi.op == "for_start" || siyuanshi.op == "while_start" || for_siyuanshi_stack.size() > 0) {
			map<string, list<Siyuanshi>> for_siyuanshi_map;
			list<Siyuanshi> tiaojian_siyuanshi_list;
			list<Siyuanshi> body_siyuanshi_list;
			if (siyuanshi.op == "for_start" || siyuanshi.op == "while_start") {
				siyuanshi = siyuanshi_list.back();
				siyuanshi_list.pop_back();
				assert(siyuanshi.op == "set_label");
				while (siyuanshi.op != "BNZ_jump" && siyuanshi.op != "BZ_jump") {
					tiaojian_siyuanshi_list.push_front(siyuanshi);
					siyuanshi = siyuanshi_list.back();
					siyuanshi_list.pop_back();
				}
				// 跳转取反
				if (siyuanshi.op == "BNZ_jump") {
					siyuanshi.op = "BZ_jump";
				}
				else {
					siyuanshi.op = "BNZ_jump";
				}
				tiaojian_siyuanshi_list.push_front(siyuanshi);
				for_siyuanshi_map["tiaojian"] = tiaojian_siyuanshi_list;
				siyuanshi = siyuanshi_list.back();
				siyuanshi_list.pop_back();
			}
			else {
				for_siyuanshi_map = for_siyuanshi_stack.back();
				tiaojian_siyuanshi_list = for_siyuanshi_stack.back()["tiaojian"];
				body_siyuanshi_list = for_siyuanshi_stack.back()["body"];
				for_siyuanshi_stack.pop_back();
			}
			while (siyuanshi.result != tiaojian_siyuanshi_list.back().result) {
				if (siyuanshi.op == "for_start") {
					break;
				}
				body_siyuanshi_list.push_front(siyuanshi);
				siyuanshi = siyuanshi_list.back();
				siyuanshi_list.pop_back();
			}
			for_siyuanshi_map["body"] = body_siyuanshi_list;
			if (siyuanshi.result != tiaojian_siyuanshi_list.back().result) {
				assert(siyuanshi.op == "for_start");
				siyuanshi_list.push_back(siyuanshi);
				for_siyuanshi_stack.push_back(for_siyuanshi_map);
				if (DEBUG_FOR == 1) {
					cout << "for_for: " << for_siyuanshi_stack.size() << endl;
				}
				continue;
			}
			// 开始按顺序输入
			if (for_siyuanshi_stack.size() > 0) {
				list<Siyuanshi> tmp_body_list = for_siyuanshi_stack.back()["body"];
				tmp_body_list.push_front(siyuanshi);
				if (DEBUG_FOR == 1) {
					//cout << tmp_body_list.front().output_string() << endl;
				}
				siyuanshi = siyuanshi_list.back();
				siyuanshi_list.pop_back();
				tmp_body_list.push_front(siyuanshi);
				if (DEBUG_FOR == 1) {
					//cout << tmp_body_list.front().output_string() << endl;
				}
				while (body_siyuanshi_list.size() > 0) {
					siyuanshi = body_siyuanshi_list.back();
					body_siyuanshi_list.pop_back();
					tmp_body_list.push_front(siyuanshi);
					if (DEBUG_FOR == 1) {
						//cout << tmp_body_list.front().output_string() << endl;
					}
				}
				while (tiaojian_siyuanshi_list.size() > 0) {
					siyuanshi = tiaojian_siyuanshi_list.back();
					tiaojian_siyuanshi_list.pop_back();
					tmp_body_list.push_front(siyuanshi);
					if (DEBUG_FOR == 1) {
						//cout << tmp_body_list.front().output_string() << endl;
					}
				}
				for_siyuanshi_stack.back()["body"] = tmp_body_list;
			}
			else {
				tmp_siyuanshi_list.push_front(siyuanshi);
				siyuanshi = siyuanshi_list.back();
				siyuanshi_list.pop_back();
				tmp_siyuanshi_list.push_front(siyuanshi);
				while (body_siyuanshi_list.size() > 0) {
					siyuanshi = body_siyuanshi_list.back();
					body_siyuanshi_list.pop_back();
					tmp_siyuanshi_list.push_front(siyuanshi);
				}
				while (tiaojian_siyuanshi_list.size() > 0) {
					siyuanshi = tiaojian_siyuanshi_list.back();
					tiaojian_siyuanshi_list.pop_back();
					tmp_siyuanshi_list.push_front(siyuanshi);
				}
			}
		}
		else {
			tmp_siyuanshi_list.push_front(siyuanshi);
		}
	}
}

void optimal() {
	reg_optimal();
	inline_init();
	inline_optimal();
	///*
	for_optimal();
	while (tmp_siyuanshi_list.size() > 0) {
		Siyuanshi siyuanshi = tmp_siyuanshi_list.back();
		tmp_siyuanshi_list.pop_back();
		siyuanshi_list.push_front(siyuanshi);
	}
	//*/
}