#include "import.h"

bool import_log(string filename)
{
	string buffer;
	string line;
	string value;
	size_t pos = 0, pos_end;
	size_t count = 0;
	vector<string> fields;
	
	if(filename.empty()) return false;
	
	if(file_read_all(filename, buffer))
	{
		db->Open("cmdw_report.db");
		
		// Turn off sync to make insert faster (50 times)
		db->ExecuteUpdate("PRAGMA SYNCHRONOUS=OFF");
		
		cout.flush();
		
		while(pos != string::npos)
		{
			//find line break
			pos_end = buffer.find_first_of("\r\n", pos);
			
			line = buffer.substr(pos, pos_end - pos);
			
			//find next line
			pos = buffer.find_first_not_of("\r\n", pos_end);
			
			if(line[0] != '#') continue;
			
			if(count % 100 == 0)
			{
				cout << ".";
				cout.flush();
			}
				
			fields.clear();
			
			line_to_fields(line, fields);
			
			if(fields[0] == "#VG#")
			{
				insert_vg_record(fields);
				count++;
			}
			else if(fields[0] == "#VGFS#")
			{
				insert_vg_fs_record(fields);
				count++;
			}
			else if(fields[0] == "#FS#")
			{
				insert_fs_record(fields);
				count++;
			}
			else if(fields[0] == "#TS#")
			{
				insert_ts_record(fields);
				count++;
			}
		}
		
		db->ExecuteUpdate("PRAGMA SYNCHRONOUS=FULL");
		db->Close();
		
		cout << endl << "Imported " << count << " records." << endl;
		
		return true;
	}

	return false;
}

bool insert_vg_record(vector<string> &fields)
{
	string sql;
	
	sql = string("insert into vg values (")
		+ "'" + fields[1] + "', "
		+ "'" + fields[2] + "', "
		+ "'" + fields[3] + "', "
		+ "'" + fields[4] + "', "
		+ "'" + fields[5] + "', "
		+ "'" + fields[6] + "')";
	
	if(db->ExecuteUpdate(sql.c_str()) != 0)
	{
		cout << db->GetErrorMsg() << endl;
		return false;
	}
	
	return true;
}

bool insert_vg_fs_record(vector<string> &fields)
{
	string sql;
	
	sql = string("insert into vg_fs values (")
		+ "'" + fields[1] + "', "
		+ "'" + fields[2] + "', "
		+ "'" + fields[3] + "', "
		+ "'" + fields[4] + "', "
		+ "'" + fields[5] + "', "
		+ "'" + fields[6] + "', "
		+ "'" + fields[7] + "', "
		+ "'" + fields[8] + "', "
		+ "'" + fields[9] + "')";
	
	if(db->ExecuteUpdate(sql.c_str()) != 0)
	{
		cout << db->GetErrorMsg() << endl;
		return false;
	}
	
	return true;
}

bool insert_fs_record(vector<string> &fields)
{
	string sql;
	
	sql = string("insert into fs values (")
		+ "'" + fields[1] + "', "
		+ "'" + fields[2] + "', "
		+ "'" + fields[3] + "', "
		+ "'" + fields[4] + "', "
		+ "'" + fields[5] + "', "
		+ "'" + fields[6] + "')";
	
	if(db->ExecuteUpdate(sql.c_str()) != 0)
	{
		cout << db->GetErrorMsg() << endl;
		return false;
	}
	
	return true;
}

bool insert_ts_record(vector<string> &fields)
{
	string sql;
	
	sql = string("insert into ts values (")
		+ "'" + fields[1] + "', "
		+ "'" + fields[2] + "', "
		+ "'" + fields[3] + "', "
		+ "'" + fields[4] + "', "
		+ "'" + fields[5] + "', "
		+ "'" + fields[6] + "', "
		+ "'" + fields[7] + "', "
		+ "'" + fields[8] + "', "
		+ "'" + fields[9] + "', "
		+ "'" + fields[10] + "', "
		+ "'" + fields[11] + "')";
	
	if(db->ExecuteUpdate(sql.c_str()) != 0)
	{
		cout << db->GetErrorMsg() << endl;
		return false;
	}
	
	return true;
}

size_t line_to_fields(string &line, vector<string> &fields)
{
	string field;
	size_t pos = 0, pos_end;
	
	while(pos != string::npos)
	{
		pos_end = line.find(' ', pos);
		
		field = line.substr(pos, pos_end - pos);
		
		fields.push_back(field);
		
		//cout << fields.size() << "\t '" << field << "'" << endl;
		
		pos = line.find_first_not_of(' ', pos_end);		
	}
	
	return fields.size();
}

bool file_read_all(string filename, string &buffer)
{
	char *_buffer;
	size_t size;
	ifstream file;

	file.open(filename.c_str(), ifstream::in);

	if(file.good())
	{
		size = file_size(file);

		_buffer = new char[size];

		file.read(_buffer, size);
		file.close();

		buffer.reserve(size);
		buffer = _buffer;
		delete[] _buffer;

		return true;
	}

	return false;
}

size_t file_size(ifstream &file)
{
	size_t size = 0;

	file.seekg(0, ios_base::end);
	size = file.tellg();
	file.seekg(0, ios_base::beg);

	return size;
}
