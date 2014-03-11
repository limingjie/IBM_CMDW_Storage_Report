////////////////////////////////////////////////////////////////////////////////
//
// SQLite3Database.h
//  
//  Description - A interface for SQLite3 database
//  Author      - Martin
//  Created     - 2007/10/31
//  Licence     - wxWidgets parts obey wxWidgets Licence 
//  Copyright   - Free
//  Changes     -
//    2007/10/31 -
//      Basic SQLite3Database properties & functions defined
//    2007/11/02 -
//      1. Fixed GetErrorMsg()
//      2. Remove support of C++ string, string must be converted by c_str()
//      3. Add functions BindwxControlWithItems(), BindwxComboBox()
//      4. Implement class SQLite3Reader
//      5. Changed all column_index from int to unsigned int
//  Problems    -
//    2007/10/31 - Fixed on 2007/11/02
//      GetErrorMsg(), local error message cannot be returned due to
//      sqlite3_errcode() not defined when recent execution is successful
//
////////////////////////////////////////////////////////////////////////////////

#ifndef __SQLITE3_DATABASE_H__
#define __SQLITE3_DATABASE_H__

//Include sqlite3 header file.
extern "C" {
#	include "sqlite3.h"
}

class SQLite3Reader
{
private :
	sqlite3 *m_db;
	sqlite3_stmt *m_stmt;

public :
	SQLite3Reader(sqlite3 *m_db, sqlite3_stmt *stmt);
	~SQLite3Reader();
	
	/// Move to next record
	/// Return false if no record left
	int Read();
	
	/// Get value from current record
	/// Return NULL or 0 if index overflow
	const char *GetString(unsigned int index = 0);
	int         GetInt(unsigned int index = 0);
	long long   GetInt64(unsigned int index = 0);
	double      GetDouble(unsigned int index = 0);

	/// Return SQLite3 error_msg
	const char *GetErrorMsg();

}; //class SQLite3Reader

class SQLite3Database
{
private :
	sqlite3 *m_db;

	/// Error code, used in this class only
	enum
	{
		_SQLITE3_DB_ERROR_           = -1,
		_SQLITE3_DB_OK_              =  0,
		_SQLITE3_DB_INDEX_OVERFLOW_  =  1,
		_SQLITE3_DB_NO_ROW_RETURN_   =  2
	};
	
	/// Error code, either from SQLite3 or from this class
	/// If error comes from SQLite3, value is _SQLITE3_DB_ERROR_
	/// If the most recent operation executed successfully, code not defined
	int m_errcode;

	int Check();

#ifdef __WXMSW__
	int BindwxControlWithItems(wxControlWithItems *ctrlwithitem, 
							   const char *sql, unsigned int index = 0);
#endif

public :
	SQLite3Database();
	~SQLite3Database();

	/// Open a SQLite3 database
	/// return true if database has been opened, false if not
	int Open(const char *filename);

	/// Close a SQLite3 database
	void Close();

	/// Execute a Update, Insert or Delete statement
	/// return true if execute successfully, false if not
	int ExecuteUpdate(const char *sql);

	/// Execute a Select statement and return a SQLite3Reader
	SQLite3Reader *ExecuteQuery(const char *sql);

	/// Return Error Message
	/// If it is a SQLite3 error, Return SQLite3 error_msg
	/// If not a SQLite3 error, return error defined in this class
	/// If the most recent execution is successful, behavior not defined
	const char *GetErrorMsg();

#ifdef __WXMSW__

	/// Bind query result to wxTextCtrl
	/// param - textctrl,     wxTextCtrl which data bind to
	/// param - sql,          SQL statement
	/// param - column_index, the index of column that will bind to textctrl
	int BindwxTextCtrl(wxTextCtrl *textctrl, const char *sql, unsigned int index = 0);

	/// Bind query result to wxTextCtrl
	/// param - combobox,     wxComboBox which data bind to
	/// param - sql,          SQL statement
	/// param - column_index, the index of column that will bind to textctrl
	int BindwxComboBox(wxComboBox *combobox, const char *sql, unsigned int index = 0);

#endif //__WXMSW__
}; //class SQLite3Database

#endif //__SQLITE3_DATABASE_H_
