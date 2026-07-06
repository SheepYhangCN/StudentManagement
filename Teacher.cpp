#include <iostream>
#include <filesystem>
#include "libs/sqlite/sqlite3.h" // https://sqlite.org/
#include "Teacher.h"
#include "Students.h"
#include <conio.h>

#include "StudentManagement.h"

using namespace std;

namespace Teacher
{
	bool password_checked = false;
	std::string new_password = "";
	std::string new_name = "";
	void main()
	{
		int selected;
		// 教师选单
		while (true)
		{
			cout << " === 教师 选单 === " << endl;
			cout << "[1] 学生数据" << endl;
			cout << "[2] 修改账户信息" << endl;
			cout << "[0] 登出" << endl;
			cin >> selected;
			cin.ignore(numeric_limits<streamsize>::max(), '\n');
			switch (selected)
			{
			default:
				break; // 回到while开头
			case 0:
				return; // 登出
			case 1:
				students();
				break;
			case 2:
				edit_account();
				break;
			}
			cout << endl;
		}
		return;
	}
	void students()
	{
		Students::init_classes_db();
		Students::menu(false);
		Students::close_classes_db();
		return;
	}
	void edit_account()
	{
		int selected;
		while (true)
		{
			cout << " === 修改账户信息 === " << endl;
			cout << "[1] 修改密码" << endl;
			cout << "[2] 修改姓名" << endl;
			cout << "[0] 返回" << endl;
			cin >> selected;
			cin.ignore(numeric_limits<streamsize>::max(), '\n');
			switch (selected)
			{
			default:
				break; // 回到while开头
			case 0:
				return; // 登出
			case 1:
				{
					password_checked = false;
					new_password = "";
					string sql_query = "UPDATE accounts SET pass = '";
					password_checked = false;
					cout << "请输入原密码：";
					new_password = sha256(password_input());
					sqlite3_exec(accounts_db, ("SELECT pass FROM accounts WHERE account = '" + account + "';").c_str(), check_password, nullptr, nullptr);
					if (!password_checked)
					{
						return;
					}
					sql_query += new_password + "' WHERE account == '" + account + "'";
					char* err = nullptr;
					sqlite3_exec(accounts_db, sql_query.c_str(), nullptr, nullptr, &err);
					if (err)
					{
						cout << "错误：" << err << endl;
						sqlite3_free(err);
					}
					else
					{
						cout << "编辑成功" << endl;
					}
				}
				break;
			case 2:
				{
					new_name = "";
					cout << "请输入新姓名：";
					getline(cin, new_name);
					string sql_query = "SELECT name FROM accounts WHERE account == '" + account + "'";
					sqlite3_exec(accounts_db, sql_query.c_str(), update_name, nullptr, nullptr);
				}
				break;
			}
			cout << endl;
		}
		return;
	}
	int check_password(void* data, int argc, char** argv, char** col_name)
	{
		if (new_password == argv[0])
		{
			password_checked = true;
			new_password = "";
			cout << "请输入新密码：";
			new_password = sha256(password_input());
			cout << endl;
		}
		else
		{
			cout << "密码错误" << endl;
		}
		return 0;
	}
	int update_name(void* data, int argc, char** argv, char** col_name)
	{
		cout << "旧名：" << (argv[0] ? argv[0] : "") << endl;
		cout << "新名：" << new_name << endl;
		cout << "你确定要修改姓名吗？（y/n）";
		string confirm;
		getline(cin, confirm);
		if (confirm != "y")
		{
			return 0;
		}
		string sql_query = "UPDATE accounts SET name = '" + new_name + "' WHERE account == '" + account + "'";
		char* err = nullptr;
		sqlite3_exec(accounts_db, sql_query.c_str(), nullptr, nullptr, &err);
		if (err)
		{
			cout << "错误：" << err << endl;
			sqlite3_free(err);
		}
		else
		{
			cout << "编辑成功" << endl;
		}
		return 0;
	}
}