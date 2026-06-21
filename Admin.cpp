#include <iostream>
#include <filesystem>
#include "libs/sqlite/sqlite3.h" // https://sqlite.org/
#include "Admin.h"
#include <conio.h>

#include "StudentManagement.h"

using namespace std;

namespace Admin
{
	bool password_checked = false;
	string edit_value = "";
	void main()
	{
		int selected;
		// 管理选单
		while (true)
		{
			cout << " === 管理员 选单 === " << endl;
			cout << "[1] 账户数据" << endl;
			cout << "[2] 学生数据" << endl;
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
					accounts();
					break;
				case 2:
					students();
					break;
			}
			cout << endl;
		}
		return;
	}
	void accounts()
	{
		int selected;
		// 账户选单
		while (true)
		{
			cout << " === 账户数据 === " << endl;	
			cout << "[1] 查看账户数据" << endl;
			cout << "[2] 添加账户" << endl;
			cout << "[3] 编辑账户" << endl;
			cout << "[4] 删除账户" << endl;
			cout << "[5] 重置账户密码" << endl;
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
				{
					while (true)
					{
						// 查看账户数据选单
						string query = "";
						cout << " === 查看账户数据 === " << endl;
						cout << "[1] 查看所有" << endl;
						cout << "[2] 筛选身份" << endl;
						cout << "[3] 筛选班级" << endl;
						cout << "[4] 搜索账户" << endl;
						cout << "[5] 搜索姓名" << endl;
						cout << "[0] 返回" << endl;
						cin >> selected;
						cin.ignore(numeric_limits<streamsize>::max(), '\n');
						if (selected == 0)
						{
							break; // 返回上一选单
						}
						else if (selected == 1)
						{
							query = "";
						}
						else if (selected == 2)
						{
							string role;
							cout << "输入你想筛选的身份：";
							getline(cin, role);
							query = "WHERE role == '" + role + "'";
						}
						else if (selected == 3)
						{
							string _class;
							cout << "输入你想筛选的班级：";
							getline(cin, _class);
							query = "WHERE class == '" + _class + "'";
						}
						else if (selected == 4)
						{
							string account;
							cout << "输入你想搜索的账户：";
							getline(cin, account);
							query = "WHERE account == '" + account + "'";
						}
						else if (selected == 5)
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
						string sql_query = "SELECT account, name, role, class FROM accounts " + query;
						sqlite3_exec(accounts_db, sql_query.c_str(), check_result, nullptr, nullptr);
						while (true)
						{
							cout << " === 请选择你想进行的操作 === " << endl;
							cout << "[1] 添加账户" << endl;
							cout << "[2] 编辑账户" << endl;
							cout << "[3] 删除账户" << endl;
							cout << "[4] 重置账户密码" << endl;
							cout << "[0] 返回" << endl;
							cin >> selected;
							cin.ignore(numeric_limits<streamsize>::max(), '\n');
							if (selected == 0)
							{
								break; // 返回上一选单
							}
							else if (selected == 1)
							{
								add_account();
							}
							else if (selected == 2)
							{
								edit_account();
							}
							else if (selected == 3)
							{
								delete_account();
							}
							else if (selected == 4)
							{
								reset_password();
							}
							else
							{
								continue; // 回到while开头
							}
						}
					}
				}
					break;
				case 2:
					add_account();
					break;
				case 3:
					edit_account();
					break;
				case 4:
					delete_account();
					break;
				case 5:
					reset_password();
					break;
			}
			cout << endl;
		}
		return;
	}
	void add_account()
	{
		// 账户名
		string account;
		cout << "添加账户：";
		cin >> account;
		cin.ignore(numeric_limits<streamsize>::max(), '\n');
		// 密码
		string password;
		cout << "设置密码：";
		char chr;
		while (true)
		{
			chr = _getch();
			if (chr == '\r')
			{
				break;
			}
			else if (chr == '\b')
			{
				if (password != "")
				{
					password.pop_back();
					cout << "\b \b";
				}
			}
			else
			{
				cout << "*";
				password += chr;
			}
		}
		password = sha256(password);
		cout << endl;
		// 身份
		string role;
		cout << "设置身份（admin / teacher）：";
		getline(cin, role);
		// 姓名
		string name;
		cout << "设置姓名：";
		getline(cin, name);
		// 班级
		string _class;
		cout << "设置班级：";
		getline(cin, _class);
		string sql_query = (string)"INSERT INTO accounts(account, pass, role" + (name == "" ? "" : ", name") + (_class == "" ? "" : ", class") + ") VALUES ('" + account + "', '" + password + "', '" + role + "'" + (name == "" ? "" : ", '" + name + "'") + (_class == "" ? "" : ", '" + _class + "'") + ");";
		char* err = nullptr;
		sqlite3_exec(accounts_db, sql_query.c_str(), nullptr, nullptr, &err);
		if (err)
		{
			cout << "错误：" << err << endl;
			sqlite3_free(err);
		}
		else
		{
			cout << "添加成功" << endl;
		}
		return;
	}
	void edit_account()
	{
		string account;
		cout << "你想编辑的账户：";
		cin >> account;
		cin.ignore(numeric_limits<streamsize>::max(), '\n');
		// 查询信息并确认
		string sql_query = "SELECT account, name, role, class FROM accounts WHERE account == '" + account + "'";
		sqlite3_exec(accounts_db, sql_query.c_str(), check_result, nullptr, nullptr);
		string confirm;
		cout << "你确定要编辑以上账户吗？（y/n）";
		getline(cin, confirm);
		if (confirm != "y")
		{
			return;
		}
		// 编辑操作
		sql_query = "UPDATE accounts SET ";
		vector<string> availables = { "pass", "role", "name", "class" };
		while (true)
		{
			// 数据
			string edit;
			cout << "请输入要修改的数据（pass, role, name, class），直接回车以开始修改：";
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
			edit_value = "";
			// 密码
			if (edit == "pass")
			{
				password_checked = false;
				cout << "请输入原密码：";
				char chr;
				while (true)
				{
					chr = _getch();
					if (chr == '\r')
					{
						break;
					}
					else if (chr == '\b')
					{
						if (edit_value != "")
						{
							edit_value.pop_back();
							cout << "\b \b";
						}
					}
					else
					{
						cout << "*";
						edit_value += chr;
					}
				}
				cout << endl;
				edit_value = sha256(edit_value);
				sqlite3_exec(accounts_db, ("SELECT pass FROM accounts WHERE account = '" + account + "';").c_str(), check_password, nullptr, nullptr);
				if (!password_checked)
				{
					continue;
				}
			}
			else // 非密码
			{
				string edit_value = "";
				cout << "请输入要修改为的值：";
				getline(cin, edit_value);
			}
			sql_query += edit + " = '" + edit_value + "',";
		}
		if (sql_query.ends_with(","))
		{
			sql_query.pop_back();
		}
		if (sql_query == "UPDATE accounts SET ") // 在没有任何东西输入时
		{
			return;
		}
		sql_query += " WHERE account == '" + account + "'";
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
		return;
	}
	void delete_account()
	{
		string account;
		cout << "你想删除的账户：";
		cin >> account;
		cin.ignore(numeric_limits<streamsize>::max(), '\n');
		// 查询信息并确认
		string sql_query = "SELECT account, name, role, class FROM accounts WHERE account == '" + account + "'";
		sqlite3_exec(accounts_db, sql_query.c_str(), check_result, nullptr, nullptr);
		cout << "你确定要删除以上账户吗？（yes/no）";
		string confirm;
		getline(cin, confirm);
		if (confirm != "yes")
		{
			return;
		}
		// 删除操作
		sql_query = "DELETE FROM accounts WHERE account == '" + account + "'";
		char* err = nullptr;
		sqlite3_exec(accounts_db, sql_query.c_str(), nullptr, nullptr, &err);
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
	void reset_password()
	{
		string account;
		cout << "你想重置密码的账户：";
		cin >> account;
		cin.ignore(numeric_limits<streamsize>::max(), '\n');
		// 查询信息并确认
		string sql_query = "SELECT account, name, role, class FROM accounts WHERE account == '" + account + "'";
		sqlite3_exec(accounts_db, sql_query.c_str(), check_result, nullptr, nullptr);
		string confirm;
		cout << "你确定要重置以上账户的密码吗？（yes/no）";
		getline(cin, confirm);
		if (confirm != "yes")
		{
			return;
		}
		// 操作
		sql_query = "UPDATE accounts SET pass = '" + sha256("") + "' WHERE account == '" + account + "'";
		char* err = nullptr;
		sqlite3_exec(accounts_db, sql_query.c_str(), nullptr, nullptr, &err);
		if (err)
		{
			cout << "错误：" << err << endl;
			sqlite3_free(err);
		}
		else
		{
			cout << "重置完成，使用空白密码可重新登录账户\n请尽快登录账户手动设置新密码" << endl;
		}
		return;
	}
	int check_result(void* data, int argc, char** argv, char** col_name)
	{
		/*for (int i = 0; i < argc; i++)
		{
			cout << col_name[i] << ": " << (argv[i] ? argv[i] : "NULL") << endl;
		}*/
		cout << "====================" << endl;
		cout << "账户：" << argv[0] << endl;
		if (argv[1])
		{
			cout << "姓名：" << argv[1] << endl;
		}
		cout << "身份：" << argv[2] << endl;
		if (argv[3])
		{
			cout << "班级：" << argv[3] << endl;
		}
		return 0;
	}
	int check_password(void* data, int argc, char** argv, char** col_name)
	{
		if (edit_value == argv[0])
		{
			password_checked = true;
			edit_value = "";
			cout << "请输入新密码：";
			char chr;
			while (true)
			{
				chr = _getch();
				if (chr == '\r')
				{
					break;
				}
				else if (chr == '\b')
				{
					if (edit_value != "")
					{
						edit_value.pop_back();
						cout << "\b \b";
					}
				}
				else
				{
					cout << "*";
					edit_value += chr;
				}
			}
			edit_value = sha256(edit_value);
			cout << endl;
		}
		else
		{
			cout << "密码错误" << endl;
		}
		return 0;
	}
	void students()
	{
		return; // todo
	}
}
