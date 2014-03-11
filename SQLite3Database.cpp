////////////////////////////////////////////////////////////////////////////////
//
// SQLite3Database.cpp
//  
//  Description - A interface for SQLite3 database
//  Author      - Martin
//  Created     - 2007/10/31
//  Licence     - wxWidgets parts obey wxWidgets Licence 
//  Copyright   - Free
//
////////////////////////////////////////////////////////////////////////////////

#include "SQLite3Database.h"

////////////////////////////////////////////////////////////////////////////////
// SQLite3Reader
////////////////////////////////////////////////////////////////////////////////
SQLite3Reader::SQLite3Reader(sqlite3 *db, sqlite3_stmt *stmt)
{
	m_db = db;
	m_stmt = stmt;
}

SQLite3Reader::~SQLite3Reader()
{
	sqlite3_finalize(m_stmt);
}

int SQLite3Reader::Read()
{
	int rc = sqlite3_step(m_stmt);
	
	return (rc == SQLITE_ROW);
}

const char *SQLite3Reader::GetString(unsigned int index)
{
	return (const char *)sqlite3_column_text(m_stmt, index);
}

int SQLite3Reader::GetInt(unsigned int index)
{
	return sqlite3_column_int(m_stmt, index);
}

long long SQLite3Reader::GetInt64(unsigned int index)
{
	return sqlite3_column_int64(m_stmt, index);
}

double SQLite3Reader::GetDouble(unsigned int index)
{
	return sqlite3_column_double(m_stmt, index);
}

const char *SQLite3Reader::GetErrorMsg()
{
	return (const char *)sqlite3_errmsg(m_db);
}
////////////////////////////////////////////////////////////////////////////////
// SQLite3Database
////////////////////////////////////////////////////////////////////////////////

SQLite3Database::SQLite3Database()
{
	m_db = 0;
	m_errcode = _SQLITE3_DB_OK_;
}

SQLite3Database::~SQLite3Database()
{
	Close();
}

int SQLite3Database::Check()
{
	return (m_db != 0);
}

int SQLite3Database::Open(const char *filename)
{
	int rc = sqlite3_open(filename, &m_db);
	
	if(rc != SQLITE_OK)
	{
		Close();
		m_errcode = _SQLITE3_DB_ERROR_;
		return -1;
	}
	
	return 0;		
}

void SQLite3Database::Close()
{
	if(!m_db)
	{
		sqlite3_close(m_db);
		m_db = 0;
	}
}

int SQLite3Database::ExecuteUpdate(const char *sql)
{
	int rc = sqlite3_exec(m_db, sql, 0, 0, 0);
	
	if(rc != SQLITE_OK)
	{
		m_errcode = _SQLITE3_DB_ERROR_;
		return -1;
	}
	
	return 0;
}

SQLite3Reader *SQLite3Database::ExecuteQuery(const char *sql)
{
	int rc;
	sqlite3_stmt *stmt;
	
	rc = sqlite3_prepare_v2(m_db, sql, -1, &stmt, 0);
	
	if(rc != SQLITE_OK)
	{
		sqlite3_finalize(stmt);
		m_errcode = _SQLITE3_DB_ERROR_;
		return 0;
	}
	else if(rc == SQLITE_DONE)
	{
		sqlite3_finalize(stmt);
		m_errcode = _SQLITE3_DB_NO_ROW_RETURN_;
		return 0;
	}
	
	return new SQLite3Reader(m_db, stmt);
}

const char *SQLite3Database::GetErrorMsg()
{
	const char *errmsg = 0;
	
	switch(m_errcode)
	{
	case _SQLITE3_DB_ERROR_:
		errmsg = (const char *)sqlite3_errmsg(m_db);
		break;
	case _SQLITE3_DB_OK_: 
		errmsg = "This is not an error.";
		break;
	case _SQLITE3_DB_INDEX_OVERFLOW_: 
		errmsg = "Index overflow.";
		break;
	}
	
	return errmsg;
}

#ifdef __WXMSW__
int SQLite3Database::
	BindwxControlWithItems(wxControlWithItems *ctrlwithitem, 
						   const char *sql, unsigned int index)
{
	int rc;
	sqlite3_stmt *stmt;
	
	ctrlwithitem->Clear();

	rc = sqlite3_prepare_v2(m_db, sql, -1, &stmt, 0);
	if(rc != SQLITE_OK)
	{
		sqlite3_finalize(stmt);
		return -1;
	}
	
	rc = sqlite3_step(stmt);
	while(rc == SQLITE_ROW)
	{
		unsigned int column_count = (unsigned int)sqlite3_column_count(stmt);
		const char *text = 0;
		
		if(index < 0 || index >= column_count)
		{
			m_errcode = _SQLITE3_DB_INDEX_OVERFLOW_;
			return -1;
		}
		
		text = (const char *)sqlite3_column_text(stmt, index);
		ctrlwithitem->Append(wxString::FromAscii(text));
		
		rc = sqlite3_step(stmt);
	}
	
	if (rc == SQLITE_DONE)
	{
		rc = 0;
	}
	else
	{	
		rc = -1;
	}
	
	sqlite3_finalize(stmt);
	
	return rc;
}

int SQLite3Database::
	BindwxTextCtrl(wxTextCtrl *textctrl, const char *sql, unsigned int index)
{
	int rc;
	sqlite3_stmt *stmt;
	
	textctrl->Clear();
		
	rc = sqlite3_prepare_v2(m_db, sql, -1, &stmt, 0);
	if(rc != SQLITE_OK)
	{
		sqlite3_finalize(stmt);
		m_errcode = _SQLITE3_DB_ERROR_;
		return -1;
	}
	
	rc = sqlite3_step(stmt);	
	if (rc == SQLITE_DONE)
	{
		rc = 0;
	}
	else if(rc == SQLITE_ROW)
	{
		unsigned int column_count = (unsigned int)sqlite3_column_count(stmt);
		const char *text = 0;
		
		if(index < 0 || index >= column_count)
		{
			m_errcode = _SQLITE3_DB_INDEX_OVERFLOW_;
			return -1;
		}
		
		text = (const char *)sqlite3_column_text(stmt, index);
		textctrl->AppendText(wxString::FromAscii(text));
		
		rc = 0;
	}
	else
	{	
		m_errcode = _SQLITE3_DB_ERROR_;
		rc = -1;
	}
	
	sqlite3_finalize(stmt);
	
	return rc;
}

int SQLite3Database::
	BindwxComboBox(wxComboBox *combobox, const char *sql, unsigned int index)
{
	return BindwxControlWithItems((wxControlWithItems *)combobox, sql, index);
}

#endif //__WXMSW__
