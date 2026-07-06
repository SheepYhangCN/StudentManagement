#include <iostream>
#include <algorithm>
#include <filesystem>
#include "libs/sqlite/sqlite3.h" // https://sqlite.org/
#include "Students.h"
#include <conio.h>

#include "StudentManagement.h"

using namespace std;

namespace Students
{
	sqlite3* classes_db = nullptr;
	string check_result_output = "";
	string current_student_table = "";
	void init_classes_db()
	{
		filesystem::path db_path = filesystem::current_path() / "classes_db.sqlite";
		if (sqlite3_open(db_path.string().c_str(), &classes_db) != SQLITE_OK)
		{
			cout << "打开学生数据库失败：" << sqlite3_errmsg(classes_db) << endl;
			classes_db = nullptr;
		}
	}
	void close_classes_db()
	{
		if (classes_db)
		{
			sqlite3_close(classes_db);
			classes_db = nullptr;
		}
	}
	string format_student_id(string table, int num)
	{
		string class_name = table.substr(5);
		string class_part = "";
		if (class_name.size() == 1)
		{
			class_part = "0" + class_name;
		}
		else
		{
			class_part = class_name;
		}
		string num_part = (num < 10 ? "0" : "") + to_string(num);
		return class_part + num_part;
	}
	string get_table_name()
	{
		string _class;
		cout << "请输入班级：";
		getline(cin, _class);
		if (_class.find("class") == 0)
		{
			return _class;
		}
		return "class" + _class;
	}
	void admin_menu()
	{
		int selected;
		// 学生选单
		while (true)
		{
			cout << " === 学生数据 === " << endl;
			cout << "[1] 查看学生数据" << endl;
			cout << "[2] 添加学生" << endl;
			cout << "[3] 编辑学生" << endl;
			cout << "[4] 删除学生" << endl;
			cout << "[0] 返回" << endl;
			cin >> selected;
			cin.ignore(numeric_limits<streamsize>::max(), '\n');
			switch (selected)
			{
				default:
					break; // 回到while开头
				case 0:
					return; // 返回上一选单
				case 1:
					view_menu();
					break;
				case 2:
					add_student();
					break;
				case 3:
					edit_menu();
					break;
				case 4:
					delete_menu();
					break;
			}
			cout << endl;
		}
		return;
	}
	void teacher_menu()
	{
		int selected;
		// 学生选单
		while (true)
		{
			cout << " === 学生数据 === " << endl;
			cout << "[1] 查看学生数据" << endl;
			cout << "[2] 编辑学生" << endl;
			cout << "[0] 返回" << endl;
			cin >> selected;
			cin.ignore(numeric_limits<streamsize>::max(), '\n');
			switch (selected)
			{
				default:
					break; // 回到while开头
				case 0:
					return; // 返回上一选单
				case 1:
					view_menu();
					break;
				case 2:
					edit_menu();
					break;
			}
			cout << endl;
		}
		return;
	}
	void menu(bool is_admin)
	{
		if (is_admin)
		{
			admin_menu();
		}
		else
		{
			teacher_menu();
		}
		return;
	}
	void view_menu()
	{
		int selected;
		while (true)
		{
			cout << " === 查看学生数据 === " << endl;
			cout << "[1] 按班级筛选" << endl;
			cout << "[2] 搜索完整学号" << endl;
			cout << "[0] 返回" << endl;
			cin >> selected;
			cin.ignore(numeric_limits<streamsize>::max(), '\n');
			switch (selected)
			{
				default:
					break; // 回到while开头
				case 0:
					return; // 返回上一选单
				case 1:
					view_by_class();
					break;
				case 2:
					view_studentid();
					break;
			}
			cout << endl;
		}
		return;
	}
	void view_by_class()
	{
		string table = get_table_name();
		current_student_table = table;
		int selected;
		while (true)
		{
			cout << " === 查看学生数据 === " << endl;
			cout << "[1] 查看所有" << endl;
			cout << "[2] 搜索学号" << endl;
			cout << "[3] 搜索姓名" << endl;
			cout << "[0] 返回" << endl;
			cin >> selected;
			cin.ignore(numeric_limits<streamsize>::max(), '\n');
			if (selected == 0)
			{
				break; // 返回上一选单
			}
			string query = "";
			if (selected == 1)
			{
				query = "";
			}
			else if (selected == 2)
			{
				string num;
				cout << "输入你想搜索的学号：";
				getline(cin, num);
				query = "WHERE num == '" + num + "'";
			}
			else if (selected == 3)
			{
				string name;
				cout << "输入你想搜索的姓名：";
				getline(cin, name);
				query = "WHERE name == '" + name + "'";
			}
			else
			{
				continue; // 回到while开头
			}
			string sql_query = "SELECT num, name, gender, birthday, student_id FROM \"" + table + "\" " + query;
			sqlite3_exec(classes_db, sql_query.c_str(), print_student, nullptr, nullptr);
			cout << "====================" << endl;
		}
		return;
	}
	void view_studentid()
	{
		string student_id;
		cout << "输入完整学号：";
		getline(cin, student_id);
		if (student_id.size() < 2)
		{
			cout << "学号格式错误" << endl;
			return;
		}
		string table = "class" + to_string(stoi(student_id.substr(0, 2)));
		current_student_table = table;
		string sql_query = "SELECT num, name, gender, birthday, student_id FROM \"" + table + "\" WHERE student_id == '" + student_id + "'";
		sqlite3_exec(classes_db, sql_query.c_str(), print_student, nullptr, nullptr);
		cout << "====================" << endl;
		return;
	}
	void edit_menu()
	{
		int selected;
		while (true)
		{
			cout << " === 编辑学生 === " << endl;
			cout << "[1] 按班级筛选" << endl;
			cout << "[2] 搜索完整学号" << endl;
			cout << "[0] 返回" << endl;
			cin >> selected;
			cin.ignore(numeric_limits<streamsize>::max(), '\n');
			switch (selected)
			{
				default:
					break; // 回到while开头
				case 0:
					return; // 返回上一选单
				case 1:
					edit_student_by_class();
					break;
				case 2:
					edit_student_studentid();
					break;
			}
			cout << endl;
		}
		return;
	}
	void edit_student_by_class()
	{
		string table = get_table_name();
		current_student_table = table;
		int num;
		cout << "学号：";
		cin >> num;
		if (cin.fail())
		{
			cin.clear();
			cin.ignore(numeric_limits<streamsize>::max(), '\n');
			cout << "请输入有效的数字" << endl;
			return;
		}
		cin.ignore(numeric_limits<streamsize>::max(), '\n');
		edit_student(table, format_student_id(table, num));
		return;
	}
	void edit_student_studentid()
	{
		string student_id;
		cout << "输入完整学号：";
		getline(cin, student_id);
		if (student_id.size() < 2)
		{
			cout << "学号格式错误" << endl;
			return;
		}
		string table = "class" + to_string(stoi(student_id.substr(0, 2)));
		current_student_table = table;
		edit_student(table, student_id);
		return;
	}
	void delete_menu()
	{
		int selected;
		while (true)
		{
			cout << " === 删除学生 === " << endl;
			cout << "[1] 按班级筛选" << endl;
			cout << "[2] 搜索完整学号" << endl;
			cout << "[0] 返回" << endl;
			cin >> selected;
			cin.ignore(numeric_limits<streamsize>::max(), '\n');
			switch (selected)
			{
				default:
					break; // 回到while开头
				case 0:
					return; // 返回上一选单
				case 1:
					delete_student_by_class();
					break;
				case 2:
					delete_student_studentid();
					break;
			}
			cout << endl;
		}
		return;
	}
	void delete_student_by_class()
	{
		string table = get_table_name();
		current_student_table = table;
		int num;
		cout << "学号：";
		cin >> num;
		if (cin.fail())
		{
			cin.clear();
			cin.ignore(numeric_limits<streamsize>::max(), '\n');
			cout << "请输入有效的数字" << endl;
			return;
		}
		cin.ignore(numeric_limits<streamsize>::max(), '\n');
		delete_student(table, format_student_id(table, num));
		return;
	}
	void delete_student_studentid()
	{
		string student_id;
		cout << "输入完整学号：";
		getline(cin, student_id);
		if (student_id.size() < 2)
		{
			cout << "学号格式错误" << endl;
			return;
		}
		string table = "class" + to_string(stoi(student_id.substr(0, 2)));
		current_student_table = table;
		delete_student(table, student_id);
		return;
	}
	void add_student()
	{
		string table = get_table_name();
		// 姓名
		string name;
		cout << "姓名：";
		getline(cin, name);
		// 性别
		string gender;
		cout << "性别：";
		getline(cin, gender);
		// 出生日期
		string birthday;
		cout << "出生日期（YYYY-MM-DD）：";
		getline(cin, birthday);
		// 插入获取num
		string sql_query = "INSERT INTO \"" + table + "\"(name, gender, birthday, student_id) VALUES ('" + name + "', '" + gender + "', '" + birthday + "', '0000');";
		char* err = nullptr;
		sqlite3_exec(classes_db, sql_query.c_str(), nullptr, nullptr, &err);
		if (err)
		{
			cout << "错误：" << err << endl;
			sqlite3_free(err);
			return;
		}
		int num = (int)sqlite3_last_insert_rowid(classes_db);
		string student_id = format_student_id(table, num);
		sql_query = "UPDATE \"" + table + "\" SET student_id = '" + student_id + "', num = " + to_string(num) + " WHERE rowid == " + to_string(num);
		err = nullptr;
		sqlite3_exec(classes_db, sql_query.c_str(), nullptr, nullptr, &err);
		if (err)
		{
			cout << "错误：" << err << endl;
			sqlite3_free(err);
		}
		else
		{
			cout << "添加成功" << endl;
			cout << "完整学号：" << student_id << endl;
		}
		return;
	}
	void edit_student(string table, string student_id)
	{
		// 查询信息并确认
		string sql_query = "SELECT num, name, gender, birthday, student_id FROM \"" + table + "\" WHERE student_id == '" + student_id + "'";
		check_result_output = "";
		sqlite3_exec(classes_db, sql_query.c_str(), collect_student, nullptr, nullptr);
		if (check_result_output == "")
		{
			cout << "未找到该学生" << endl;
			return;
		}
		cout << check_result_output << endl;
		// 确认
		string confirm;
		cout << "你确定要编辑以上学生吗？（y/n）";
		getline(cin, confirm);
		if (confirm != "y")
		{
			return;
		}
		// 编辑操作
		sql_query = "UPDATE \"" + table + "\" SET ";
		vector<string> availables = { "name", "gender", "birthday" };
		while (true)
		{
			// 数据
			string edit;
			cout << "请输入要修改的数据（name, gender, birthday），直接回车以开始修改：";
			getline(cin, edit);
			if (edit == "")
			{
				break;
			}
			if (find(availables.begin(), availables.end(), edit) == availables.end())
			{
				continue;
			}
			// 内容
			string edit_value = "";
			cout << "请输入新的" << edit << "：";
			getline(cin, edit_value);
			sql_query += edit + " = '" + edit_value + "',";
		}
		if (sql_query.ends_with(","))
		{
			sql_query.pop_back();
		}
		if (sql_query == "UPDATE \"" + table + "\" SET ") // 在没有任何东西输入时
		{
			return;
		}
		sql_query += " WHERE student_id == '" + student_id + "'";
		char* err = nullptr;
		sqlite3_exec(classes_db, sql_query.c_str(), nullptr, nullptr, &err);
		if (err)
		{
			cout << "错误：" << err << endl;
			sqlite3_free(err);
		}
		else
		{
			cout << "编辑成功" << endl;
		}
		return;
	}
	void delete_student(string table, string student_id)
	{
		// 查询信息并确认
		string sql_query = "SELECT num, name, gender, birthday, student_id FROM \"" + table + "\" WHERE student_id == '" + student_id + "'";
		check_result_output = "";
		sqlite3_exec(classes_db, sql_query.c_str(), collect_student, nullptr, nullptr);
		if (check_result_output == "")
		{
			cout << "未找到该学生" << endl;
			return;
		}
		cout << check_result_output << endl;
		cout << "你确定要删除以上学生吗？（y/n）";
		string confirm;
		getline(cin, confirm);
		if (confirm != "y")
		{
			return;
		}
		// 删除操作
		sql_query = "DELETE FROM \"" + table + "\" WHERE student_id == '" + student_id + "'";
		char* err = nullptr;
		sqlite3_exec(classes_db, sql_query.c_str(), nullptr, nullptr, &err);
		if (err)
		{
			cout << "错误：" << err << endl;
			sqlite3_free(err);
		}
		else
		{
			cout << "删除成功" << endl;
		}
		return;
	}
	int print_student(void* data, int argc, char** argv, char** col_name)
	{
		cout << "====================" << endl;
		if (current_student_table != "")
		{
			cout << "班级：" << current_student_table.substr(5) << endl;
		}
		cout << "学号：" << argv[0] << endl;
		if (argv[4])
		{
			cout << "完整学号：" << argv[4] << endl;
		}
		cout << "姓名：" << argv[1] << endl;
		cout << "性别：" << argv[2] << endl;
		if (argv[3])
		{
			cout << "出生日期：" << argv[3] << endl;
		}
		return 0;
	}
	int collect_student(void* data, int argc, char** argv, char** col_name)
	{
		if (argv[0])
		{
			check_result_output += "学号：" + string(argv[0]) + "  ";
		}
		if (argv[4])
		{
			check_result_output += "完整学号：" + string(argv[4]) + "  ";
		}
		if (argv[1])
		{
			check_result_output += "姓名：" + string(argv[1]) + "  ";
		}
		if (argv[2])
		{
			check_result_output += "性别：" + string(argv[2]) + "  ";
		}
		if (argv[3])
		{
			check_result_output += "出生日期：" + string(argv[3]);
		}
		return 0;
	}
}
