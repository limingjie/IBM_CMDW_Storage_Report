#ifndef __GEN_HTML_H__
#define __GEN_HTML_H__

#include <cstdio>
#include <iostream>
#include <fstream>
#include <string>
#include <algorithm>
#include <vector>

using namespace std;

#include "SQLite3Database.h"

extern SQLite3Database *db;

bool report_stg_all();

bool report_stg_summary();

bool report_stg_summary_lpar(ofstream &file, string lpar);

bool report_stg_lpar(string hostname);

bool report_ts_all();

bool report_ts_summary();

bool report_ts_summary_host(ofstream &file, string hostname);

bool report_ts_summary_chart(ofstream &file);

bool report_ts_database(string hostname, string instance, string database);

void report_index();

void report_history();

void report_html_head(ofstream &file, string title);

void report_html_tail(ofstream &file);

#endif
