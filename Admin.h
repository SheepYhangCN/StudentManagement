#pragma once

#include <string>

namespace Admin
{
	extern bool password_checked;
	extern std::string edit_value;
	void main();
	void accounts();
	void students();
	void add_account();
	void edit_account();
	void delete_account();
	void reset_password();
	int check_result(void* data, int argc, char** argv, char** col_name);
	int check_password(void* data, int argc, char** argv, char** col_name);
}