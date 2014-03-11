#include "report.h"
#include "import.h"
#include "genhtml.h"

int main(int argc, char **argv)
{
	string str;
	
	if(argc < 3)
	{
		usage();
		
		return 0;
	}
	
	if(!create_db("cmdw_report.db")) 
	{
		cout << "Failed to create DB, please report error!\n" << endl;
		
		return 0;
	}

	str = string(argv[1]);
	transform(str.begin(), str.end(), str.begin(), (int (*)(int))std::toupper);
	
	if(str == "IMPORT")
	{
		cout << "Using Import Function..." << endl;

		for(int i = 2; i < argc; i++)
		{
			cout << "Importing file: " << argv[i] << endl;
			
			if(!import_log(argv[i]))
			{
				cout << "Import failed, pls check file!" << endl;
			}			
		}
		
		return 0;
	}
	else if(str == "EXPORT")
	{	
		cout << "Using Report Function..." << endl;
		
		str = string(argv[2]);
		transform(str.begin(), str.end(), str.begin(), (int (*)(int))std::toupper);
		
		if(str == "HTML")
		{
			cout << "Generating Report..." << endl;
			
			report_stg_all();
			report_ts_all();
			report_index();
			report_history();
			
			return 0;
		}
	}
	else if(str == "SETTIME")
	{
		string time = "";
		
		for(int i = 2; i < argc; i++)
		{
			if(i != 2) time.push_back(' ');
			time += string(argv[i]);
		}
		
		cout << "Set report time to: " << time << endl;
		set_time(time);
		
		return 0;
	}
	
	usage();

	return 0;
}

void usage()
{
	cout << 
		"Usage: report import FILE\n"
		"       report settime DATETIME\n"
		"       report export HTML\n";
}

void set_time(string time)
{
	string sql;
	
	db->Open("cmdw_report.db");
	
	sql = "delete from time";
	db->ExecuteUpdate(sql.c_str());
	
	sql = "insert into time values('" + time + "')";	
	db->ExecuteUpdate(sql.c_str());
	
	db->Close();	
}

bool create_db(string name)
{
	db = new SQLite3Database();
	
	int rc = db->Open(name.c_str());
	
	rc = db->ExecuteUpdate
	(
		"create table if not exists vg "
		"( "
		"	host_name   text(10), "
		"	name		text(10), "
		"	total_space real, "
		"	free_space  real, "
		"	used_space  real, "
		"	pp_size     real "
		") "
	);
	
	if(rc != 0)
	{
		cout << db->GetErrorMsg() << endl;
		db->Close();
		return false;
	}
	
	rc = db->ExecuteUpdate
	(
		"create table if not exists vg_fs "
		"( "
		"  host_name   text(10), "
		"  vg_name     text(10), "
		"  lv_name     text(19), "
		"  type        text(10), "
		"  lps         integer , "
		"  pps         integer , "
		"  pvs         integer , "
		"  lv_state    text(15), "
		"  fs_name     text(64) "
		") "
	);
	
	if(rc != 0)
	{
		cout << db->GetErrorMsg() << endl;
		db->Close();
		return false;
	}
	
	rc = db->ExecuteUpdate
	(
		"create table if not exists fs "
		"( "
		"host_name   text(10), "
		"filesystem  text(20), "
		"mb_blocks   real, "
		"free        real, "
		"pct         text(5), "
		"name        text(64) "
		") "
	);
	
	if(rc != 0)
	{
		cout << db->GetErrorMsg() << endl;
		db->Close();
		return false;
	}
	
	rc = db->ExecuteUpdate
	(
		"create table if not exists ts "
		"( "
		"  host_name    text(10), "
		"  instance     text(8), "
		"  db_name      text(10), "
		"  node_num     integer, "
		"  ts_name      text(18), "
		"  total_space  real, "
		"  used_space   real, "
		"  free_space   real, "
		"  reduce_max   real, "
		"  reduce_max_p integer, "
		"  page_size    integer "
		") "
	);
	
	if(rc != 0)
	{
		cout << db->GetErrorMsg() << endl;
		db->Close();
		return false;
	}
	
	rc = db->ExecuteUpdate("create table if not exists time ( time text(256) )");
	
	if(rc != 0)
	{
		cout << db->GetErrorMsg() << endl;
		db->Close();
		return false;
	}
	
	db->Close();
	
	return true;
}

