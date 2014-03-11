#ifndef __REPORT_H__
#define __REPORT_H__

#include <cstdio>
#include <iostream>
#include <fstream>
#include <string>
#include <algorithm>
#include <vector>

using namespace std;

#include "SQLite3Database.h"

SQLite3Database *db;

void usage();

void set_time(string time);

bool create_db(string name);

#endif
