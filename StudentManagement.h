#pragma once

#include <string>
#include <filesystem>
#include "libs/sqlite/sqlite3.h"

extern std::string account;
extern std::string password;
extern bool logged;
extern std::filesystem::path accounts_db_path;
extern sqlite3 *accounts_db;

std::string sha256(const std::string& str);

int check_account(void* data, int argc, char** argv, char** col_name);