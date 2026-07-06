#pragma once

#include <string>
#include "libs/sqlite/sqlite3.h"

namespace Students
{
	extern sqlite3* classes_db;
	extern std::string check_result_output;

	void init_classes_db();
	void close_classes_db();

	void menu(bool is_admin);
	void admin_menu();
	void teacher_menu();

	void view_menu();
	void view_by_class();
	void view_studentid();
	void edit_menu();
	void edit_student_by_class();
	void edit_student_studentid();
	void delete_menu();
	void delete_student_by_class();
	void delete_student_studentid();

	void add_student();
	void edit_student(std::string table, std::string student_id);
	void delete_student(std::string table, std::string student_id);

	int print_student(void* data, int argc, char** argv, char** col_name);
	int collect_student(void* data, int argc, char** argv, char** col_name);
}
