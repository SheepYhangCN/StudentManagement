#pragma once
#include "libs/sqlite/sqlite3.h"

namespace Teacher
{
	extern bool password_checked;
	extern std::string new_password;
	extern std::string new_name;
	void main();
	void students();
	void edit_account();
	int check_password(void* data, int argc, char** argv, char** col_name);
	int update_name(void* data, int argc, char** argv, char** col_name);
}
