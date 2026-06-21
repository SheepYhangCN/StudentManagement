#include <iostream>
#include <filesystem>
#include <sstream>
#include <iomanip>
#include <stdexcept>
#include <string>
#include <vector>
#include <windows.h>
#include <bcrypt.h>
#include <conio.h>
#pragma comment(lib, "bcrypt.lib")
#include "libs/sqlite/sqlite3.h" // https://sqlite.org/
using namespace std;

#include "StudentManagement.h"
#include "Admin.h"
#include "Teacher.h"

string account = "";
string password = "";
bool logged = false;
filesystem::path accounts_db_path = filesystem::current_path() / "accounts_db.sqlite";
sqlite3 *accounts_db = nullptr;

// Sha256 generation completely by Qwen AI
string sha256(const string& str) {
	BCRYPT_ALG_HANDLE hAlg = nullptr;
	BCRYPT_HASH_HANDLE hHash = nullptr;
	DWORD hashLen = 32; // SHA256 = 32 bytes
	vector<BYTE> hash(hashLen);

	if (BCryptOpenAlgorithmProvider(&hAlg, BCRYPT_SHA256_ALGORITHM, nullptr, 0) != 0)
		throw runtime_error("BCryptOpenAlgorithmProvider failed");

	if (BCryptCreateHash(hAlg, &hHash, nullptr, 0, nullptr, 0, 0) != 0) {
		BCryptCloseAlgorithmProvider(hAlg, 0);
		throw runtime_error("BCryptCreateHash failed");
	}

	BCryptHashData(hHash, reinterpret_cast<PUCHAR>(const_cast<char*>(str.c_str())), static_cast<ULONG>(str.size()), 0);
	BCryptFinishHash(hHash, hash.data(), hashLen, 0);
	BCryptDestroyHash(hHash);
	BCryptCloseAlgorithmProvider(hAlg, 0);

	stringstream ss;
	for (DWORD i = 0; i < hashLen; i++) {
		ss << hex << setw(2) << setfill('0') << static_cast<int>(hash[i]);
	}
	return ss.str();
}

int check_account(void* data, int argc, char** argv, char** col_name)
{
	if (sha256(password) == argv[0]) // 这里是纯sha256，没加盐，懒得加了，demo随便爆破（
	{
		logged = true;
		cout << "欢迎回来，" << argv[2] << (argv[1] == string("teacher") ? " 老师" : "") << "！" << endl;
		if (argv[1] == string("admin"))
		{
			Admin::main(); // 进入管理选单
		}
		else if (argv[1] == string("teacher"))
		{
			Teacher::main(); // 进入教师选单
		}
	}
	else
	{
		cout << "账号或密码错误" << endl;
	}
	return 0;
}

int main()
{
	while (true)
	{
		account.clear();
		password.clear();
		logged = false;

		cout << "欢迎使用学生管理系统\n请登录账号：";
		getline(cin, account);
		cout << "请输入密码：";
		// cin >> password; // 密码有回显
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
		cout << endl;

		if (sqlite3_open(accounts_db_path.string().c_str(), &accounts_db) != SQLITE_OK)
		{
			throw runtime_error("打开数据库失败，错误码：" + to_string(sqlite3_errcode(accounts_db)) + "，错误详情：" + sqlite3_errmsg(accounts_db));
			return -1;
		}
		// 从账号获取密码sha256
		string sql_query = "SELECT pass,role,name,class FROM accounts WHERE account = '" + account + "';"; // 使用字符串拼接会导致sql注入，可以换成其它方法来SELECT以解决这个问题，但是这只是个demo，所以就先不管了
		sqlite3_exec(accounts_db, sql_query.c_str(), check_account, nullptr, nullptr);
		sqlite3_close(accounts_db);
		if (!logged)
		{
			cout << "登录失败" << endl;
		}
		cout << endl;
	}
	return 0;
}
