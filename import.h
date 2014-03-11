#ifndef __IMPORT_H__
#define __IMPORT_H__

#include <cstdio>
#include <iostream>
#include <fstream>
#include <string>
#include <algorithm>
#include <vector>

using namespace std;

#include "SQLite3Database.h"

extern SQLite3Database *db;

bool import_log(string filename);

bool insert_vg_record(vector<string> &fields);

bool insert_vg_fs_record(vector<string> &fields);

bool insert_fs_record(vector<string> &fields);

bool insert_ts_record(vector<string> &fields);

size_t line_to_fields(string &line, vector<string> &fields);

bool file_read_all(string filename, string &buffer);

size_t file_size(ifstream &file);

#endif
