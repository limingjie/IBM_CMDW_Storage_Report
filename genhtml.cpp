#include "genhtml.h"

bool report_stg_all()
{
	string sql;
	string hostname, vg, fs;

	SQLite3Reader *reader;
	
	db->Open("cmdw_report.db");
	
	sql = "select distinct host_name from vg order by host_name";
	
	reader = db->ExecuteQuery(sql.c_str());	
	while(reader->Read())
	{
		hostname = reader->GetString(0);
		
		report_stg_lpar(hostname);
	}
	delete reader;
	
	report_stg_summary();
	
	db->Close();
	
	return true;
}

bool report_stg_summary()
{
	string sql;
	string filename = "./html/STG+Summary.html";
	string title = "CMDW Storage Report - Storage Summary";

	SQLite3Reader *reader;
	ofstream file;
	
	file.open(filename.c_str(), ofstream::out);
	file.precision(2);
	file << fixed;
	
	report_html_head(file, title);

	// Report head
	file << "<tr><td class=\"title\">CMDW STORAGE REPORT - STORAGE SUMMARY</td></tr>\n";

	// table 1 - P595 CMDW 1/2, physical summary
	file <<
		"<tr><td align=\"center\">\n"
		"<table class=\"report\" cellspacing=\"0\" cellpadding=\"0\" border=\"1\" width=\"100%\">\n"
		"<tr><th>P595 Summary</th><td colspan=\"4\"></td></tr>\n"
		"<tr>\n"
		"<th width=\"20%\"></th>\n"
		"<th width=\"20%\">Raw Capacity (DOU)</th>\n"
		"<th width=\"20%\">Usable Capacity</th>\n"
		"<th width=\"20%\">Used Capacity</th>\n"
		"<th width=\"20%\">Expansion</th>\n"
		"</tr>\n";

	// p595 1
	sql = 
		"select sum(used_space) "
		"from vg "
		"where substr(host_name,1,1) = 'A' "
		"or substr(host_name,1,1) = 'B' ";

	reader = db->ExecuteQuery(sql.c_str());
	while(reader->Read())
	{
		file <<
			"<tr class=\"data\">\n"
			"<td style=\"text-align:left;\">P595 CMDW 1</td>\n"
			"<td>50TB</td>\n"
			"<td>34.29TB</td>\n"
			"<td>" << reader->GetDouble(0) / 1024.0 / 1024.0 << "TB</td>\n"
			"<td>" << 34.29 - reader->GetDouble(0) / 1024.0 / 1024.0 << "TB</td>\n"
			"</tr>\n";
	}
	delete reader;

	// p595 2
	sql = 
		"select sum(used_space) "
		"from vg "
		"where substr(host_name,1,1) = 'C' ";

	reader = db->ExecuteQuery(sql.c_str());
	while(reader->Read())
	{
		file <<
			"<tr class=\"data\">\n"
			"<td style=\"text-align:left;\">P595 CMDW 2</td>\n"
			"<td>46TB</td>\n"
			"<td>31.16TB</td>\n"
			"<td>" << reader->GetDouble(0) / 1024.0 / 1024.0 << "TB</td>\n"
			"<td>" << 31.16 - reader->GetDouble(0) / 1024.0 / 1024.0 << "TB</td>\n"
			"</tr>\n";
	}
	delete reader;

	// p595 1+2
	sql = "select sum(used_space) from vg ";

	reader = db->ExecuteQuery(sql.c_str());
	while(reader->Read())
	{
		file <<
			"<tr class=\"data\">\n"
			"<th>Summary</th>\n"
			"<th>96TB</th>\n"
			"<th>65.45TB</th>\n"
			"<th>" << reader->GetDouble(0) / 1024.0 / 1024.0 << "TB</th>\n"
			"<th>" << 65.45 - reader->GetDouble(0) / 1024.0 / 1024.0 << "TB</th>\n"
			"</tr>\n";
	}
	delete reader;

	file <<
		"</table>\n"
		"</td></tr>\n";

	// all lpar summary
	file <<
		"<tr><td align=\"center\">\n"
		"<table class=\"report\" cellspacing=\"0\" cellpadding=\"0\" border=\"1\" width=\"100%\">\n"
		"<tr><th>Summary of all LPARs</th><td colspan=\"7\"></td></tr>\n"
		"<tr>\n"
		"<th width=\"20%\">LPAR</th>\n"
		"<th width=\"20%\">Total Space</th>\n"
		"<th width=\"20%\">Used Space</th>\n"
		"<th width=\"20%\">Expansion</th>\n"
		"<th width=\"20%\">Used %</th>\n"
		"</tr>\n";
	
	sql = 
		"select host_name, sum(total_space), sum(used_space), sum(free_space) "
		"from vg "
		"group by host_name "
		"order by host_name ";
	
	reader = db->ExecuteQuery(sql.c_str());
	while(reader->Read())
	{
		file <<
			"<tr class=\"data\">\n"
			"<td style=\"text-align:left;\"><a href=\"STG+" << reader->GetString(0) << ".html\">" << reader->GetString(0) << "</a></td>\n"
			"<td>" << reader->GetDouble(1) / 1024.0 / 1024.0 << "TB</td>\n"
			"<td>" << reader->GetDouble(2) / 1024.0 / 1024.0 << "TB</td>\n"
			"<td>" << reader->GetDouble(3) / 1024.0 / 1024.0 << "TB</td>\n"
			"<td>" << reader->GetDouble(2) / reader->GetDouble(1) * 100.0 << "%</td>\n"
			"</tr>\n";
	}
	delete reader;

	sql = 
		"select sum(total_space), sum(used_space), sum(free_space) "
		"from vg ";
	
	reader = db->ExecuteQuery(sql.c_str());
	while(reader->Read())
	{
		file <<
			"<tr>"
			"<th>Summary</td>\n"
			"<th>" << reader->GetDouble(0) / 1024.0 / 1024.0 << "TB</th>\n"
			"<th>" << reader->GetDouble(1) / 1024.0 / 1024.0 << "TB</th>\n"
			"<th>" << reader->GetDouble(2) / 1024.0 / 1024.0 << "TB</th>\n"
			"<th>" << reader->GetDouble(1) / reader->GetDouble(0) * 100.0 << "%</th>\n"
			"</tr>\n";
	}
	delete reader;

	file <<
		"<tr>\n"
		"<th></th>\n"
		"<th colspan=\"4\" style=\"text-align:left;color:#ff0000\">"
		"Non-active Physical Volumes are not included in this table."
		"</th>\n"
		"<tr>\n"
		"</table>\n"
		"</td></tr>\n";

	// vg summary of lpars
	sql = "select distinct substr(host_name, 1, 6) from vg";
	
	reader = db->ExecuteQuery(sql.c_str());
	while(reader->Read())
	{
		report_stg_summary_lpar(file, reader->GetString(0));
	}
	delete reader;
	
	report_html_tail(file);
	
	file.close();
	
	cout << "Generated Report: " << filename << endl;

	return true;
}

bool report_stg_summary_lpar(ofstream &file, string lpar)
{
	string sql;
	SQLite3Reader *reader;
	
	file <<
		"<tr><td align=\"center\">\n"
		"<table class=\"report\" cellspacing=\"0\" cellpadding=\"0\" border=\"1\" width=\"100%\">\n"
		"<tr><th>" << lpar << " Summary</th><td colspan=\"7\"></td></tr>\n"
		"<tr>\n"
		"<th width=\"20%\">Category</th>\n"
		"<th width=\"20%\">Total Space</th>\n"
		"<th width=\"20%\">Used Space</th>\n"
		"<th width=\"20%\">Expansion</th>\n"
		"<th width=\"20%\">Used %</th>\n"
		"</tr>\n";
	
	// system
	sql = 
		"select sum(total_space), sum(used_space), sum(free_space) "
		"from vg "
		"where substr(host_name, 1, 6)='" + lpar + "' "
		"and name='rootvg'";
	
	reader = db->ExecuteQuery(sql.c_str());
	while(reader->Read())
	{
		file <<
			"<tr class=\"data\">\n"
			"<td style=\"text-align:left;\">System</td>\n"
			"<td>" << reader->GetDouble(0) / 1024.0 / 1024.0 << "TB</td>\n"
			"<td>" << reader->GetDouble(1) / 1024.0 / 1024.0 << "TB</td>\n"
			"<td>" << reader->GetDouble(2) / 1024.0 / 1024.0 << "TB</td>\n"
			"<td>" << reader->GetDouble(1) / reader->GetDouble(0) * 100.0 << "%</td>\n"
			"</tr>\n";
	}
	delete reader;

	// db
	sql = 
		"select sum(total_space), sum(used_space), sum(free_space) "
		"from vg "
		"where substr(host_name, 1, 6)='" + lpar + "' "
		"and name like 'bpu%vg' ";
	
	reader = db->ExecuteQuery(sql.c_str());
	while(reader->Read())
	{
		file <<
			"<tr class=\"data\">\n"
			"<td style=\"text-align:left;\">Database</td>\n"
			"<td>" << reader->GetDouble(0) / 1024.0 / 1024.0 << "TB</td>\n"
			"<td>" << reader->GetDouble(1) / 1024.0 / 1024.0 << "TB</td>\n"
			"<td>" << reader->GetDouble(2) / 1024.0 / 1024.0 << "TB</td>\n"
			"<td>" << reader->GetDouble(1) / reader->GetDouble(0) * 100.0 << "%</td>\n"
			"</tr>\n";
	}
	delete reader;

	// ds & backup
	sql = 
		"select sum(total_space), sum(used_space), sum(free_space) "
		"from vg "
		"where substr(host_name, 1, 6)='" + lpar + "' "
		"and name not like 'bpu%vg' "
		"and name <> 'rootvg'";
	
	reader = db->ExecuteQuery(sql.c_str());
	while(reader->Read())
	{
		file <<
			"<tr class=\"data\">\n"
			"<td style=\"text-align:left;\">DataStage & Backup</td>\n"
			"<td>" << reader->GetDouble(0) / 1024.0 / 1024.0 << "TB</td>\n"
			"<td>" << reader->GetDouble(1) / 1024.0 / 1024.0 << "TB</td>\n"
			"<td>" << reader->GetDouble(2) / 1024.0 / 1024.0 << "TB</td>\n"
			"<td>" << reader->GetDouble(1) / reader->GetDouble(0) * 100.0 << "%</td>\n"
			"</tr>\n";
	}
	delete reader;

	// summary
	sql = 
		"select sum(total_space), sum(used_space), sum(free_space) "
		"from vg "
		"where substr(host_name, 1, 6)='" + lpar + "' ";
		
	reader = db->ExecuteQuery(sql.c_str());
	while(reader->Read())
	{
		file <<
			"<tr>\n"
			"<th>Summary</th>\n"
			"<th>" << reader->GetDouble(0) / 1024.0 / 1024.0 << "TB</th>\n"
			"<th>" << reader->GetDouble(1) / 1024.0 / 1024.0 << "TB</th>\n"
			"<th>" << reader->GetDouble(2) / 1024.0 / 1024.0 << "TB</th>\n"
			"<th>" << reader->GetDouble(1) / reader->GetDouble(0) * 100.0 << "%</th>\n"
			"</tr>\n";
	}
	delete reader;

	file <<
		"<tr>\n"
		"<th></th>\n"
		"<th colspan=\"4\" style=\"text-align:left;color:#ff0000\">"
		"Non-active Physical Volumes are not included in this table."
		"</th>\n"
		"<tr>\n"
		"</table>\n"
		"</td></tr>\n";

	return true;
}

bool report_stg_lpar(string hostname)
{
	string sql;
	string filename = "./html/STG+" + hostname + ".html";
	string title = "FMS CMDW Storage Report: Storage+" + hostname;
	
	SQLite3Reader *reader;
	ofstream file;
	
	file.open(filename.c_str(), ofstream::out);
	file.precision(2);
	file << fixed;
	
	report_html_head(file, title);
	
	// Report head
	file << 
		"<tr><td class=\"title\">CMDW STORAGE REPORT - STORAGE</td></tr>\n"
		"<tr><td style=\"text-align:left;\">Server: <b>" << hostname << "</b></td></tr>\n";
	
	// System volume groups...
	// I am too lazy to write a subroutine to handle different types of fs...
	// It depends on project, may different anywhere.
	file <<
		"<tr><td align=\"center\">\n"
		"<table class=\"report\" cellspacing=\"0\" cellpadding=\"0\" border=\"1\" width=\"100%\">\n"
		"<tr><th>System</th><td colspan=\"7\"></td></tr>\n"
		"<tr>\n"
		"<th width=\"31%\" rowspan=\"2\">Filesystem</th>\n"
		"<th colspan=\"4\">Logical</th>\n"
		"<th colspan=\"3\">Physical</th>\n"
		"</tr>\n"
		"<tr>\n"
		"<th width=\"10%\">Size (MB)</th>\n"
		"<th width=\"9%\">Used (MB)</th>\n"
		"<th width=\"9%\">Free (MB)</th>\n"
		"<th width=\"8%\">Used%</th>\n"
		"<th width=\"8%\">Copies</th>\n"
		"<th width=\"10%\">Size (MB)</th>\n"
		"<th width=\"15%\">Volume Group</th>\n"
		"</tr>\n";
	
	sql = 
		"select fs.name, fs.mb_blocks, round(fs.mb_blocks - fs.free, 2), fs.free, fs.pct, "
		"  vg_fs.pps / vg_fs.lps, vg_fs.pps * vg.pp_size, vg.name "
		"from vg, vg_fs, fs "
		"where vg.host_name = vg_fs.host_name "
		"and vg.name = vg_fs.vg_name "
		"and vg_fs.host_name = fs.host_name "
		"and vg_fs.fs_name = fs.name "
		"and vg.host_name = '" + hostname + "' "
		"and vg_fs.vg_name = 'rootvg' "
		"order by vg.name, fs.name ";
	
	reader = db->ExecuteQuery(sql.c_str());
	while(reader->Read())
	{
		file <<
			"<tr class=\"data\">\n"
			"<td style=\"text-align:left;\">" << reader->GetString(0) << "</td>\n"
			"<td>" << reader->GetDouble(1) << "</td>\n"
			"<td>" << reader->GetDouble(2) << "</td>\n"
			"<td>" << reader->GetDouble(3) << "</td>\n"
			"<td>" << reader->GetDouble(2) / reader->GetDouble(1) * 100.0 << "%</td>\n"
			"<td>" << reader->GetInt(5) << "</td>\n"
			"<td>" << reader->GetDouble(6) << "</td>\n"
			"<td style=\"text-align:left;\">" << reader->GetString(7) << "</td>\n"
			"</tr>\n";
	}
	delete reader;
	
	sql =
		"select sum(vg_fs.lps * vg.pp_size), sum(vg_fs.pps * vg.pp_size) "
		"from vg, vg_fs "
		"where vg.host_name = vg_fs.host_name "
		"and vg.name = vg_fs.vg_name "
		"and vg.host_name = '" + hostname + "' "
		"and vg_fs.vg_name = 'rootvg' "
		"and vg_fs.fs_name not in"
		"( "
		"	select fs.name from fs, vg_fs "
		"	where fs.host_name = vg_fs.host_name "
		"	and fs.name = vg_fs.fs_name "
		"	and fs.host_name = '" + hostname + "' "
		"	and vg_fs.vg_name = 'rootvg' "
		") ";

	reader = db->ExecuteQuery(sql.c_str());
	while(reader->Read())
	{
		file <<
			"<tr style=\"background-color:#ffccaa;\">"
			"<td style=\"text-align:left;\">Other (boot, paging, sysdump, etc)</td>\n"
			"<td>" << reader->GetDouble(0) << "</td>\n"
			"<td style=\"text-align:center;\">-</td>\n"
			"<td style=\"text-align:center;\">-</td>\n"
			"<td style=\"text-align:center;\">-</td>\n"
			"<td style=\"text-align:center;\">-</td>\n"
			"<td>" << reader->GetDouble(1) << "</td>\n"
			"<td style=\"text-align:center;\">-</td>\n"
			"</tr>\n";
	}
	delete reader;

	sql =
		"select sum(vg_fs.lps * vg.pp_size), sum(vg_fs.pps * vg.pp_size), vg.name "
		"from vg, vg_fs "
		"where vg.host_name = vg_fs.host_name "
		"and vg.name = vg_fs.vg_name "
		"and vg.host_name = '" + hostname + "' "
		"and vg_fs.vg_name = 'rootvg' ";

	reader = db->ExecuteQuery(sql.c_str());
	while(reader->Read())
	{
		file <<
			"<tr>\n"
			"<th>Summary</th>\n"
			"<th>" << reader->GetDouble(0) << "</th>\n"
			"<th>-</th>\n"
			"<th>-</th>\n"
			"<th>-</th>\n"
			"<th>-</th>\n"
			"<th>" << reader->GetDouble(1) << "</th>\n"
			"<th>-</th>\n"
			"</tr>\n";
	}
	delete reader;
	
	sql = "select sum(free_space) from vg where host_name = '" + hostname + "' and name = 'rootvg'";

	reader = db->ExecuteQuery(sql.c_str());
	
	while(reader->Read())
	{
		file <<
			"<tr>\n"
			"<th>Expansion</th>\n"
			"<th colspan=\"4\">-</th>\n"
			"<th colspan=\"3\">" << reader->GetDouble(0) << "MB (VG)</th>\n"
			"</tr>\n";
	}
	delete reader;
	
	file <<
		"</table>\n"
		"</td></tr>\n";

	// Database volume groups...
	// I am too lazy to write a subroutine to handle different types of fs...
	// It depends on project, may different anywhere.
	file <<
		"<tr><td align=\"center\">\n"
		"<table class=\"report\" cellspacing=\"0\" cellpadding=\"0\" border=\"1\" width=\"100%\">\n"
		"<tr><th>Database</th><td colspan=\"7\"></td></tr>\n"
		"<tr>\n"
		"<th width=\"31%\" rowspan=\"2\">Filesystem</th>\n"
		"<th colspan=\"4\">Logical</th>\n"
		"<th colspan=\"3\">Physical</th>\n"
		"</tr>\n"
		"<tr>\n"
		"<th width=\"10%\">Size (MB)</th>\n"
		"<th width=\"9%\">Used (MB)</th>\n"
		"<th width=\"9%\">Free (MB)</th>\n"
		"<th width=\"8%\">Used%</th>\n"
		"<th width=\"8%\">Copies</th>\n"
		"<th width=\"10%\">Size (MB)</th>\n"
		"<th width=\"15%\">Volume Group</th>\n"
		"</tr>\n";
	
	sql = 
		"select fs.name, fs.mb_blocks, round(fs.mb_blocks - fs.free, 2), fs.free, fs.pct, "
		"  vg_fs.pps / vg_fs.lps, vg_fs.pps * vg.pp_size, vg.name "
		"from vg, vg_fs, fs "
		"where vg.host_name = vg_fs.host_name "
		"and vg.name = vg_fs.vg_name "
		"and vg_fs.host_name = fs.host_name "
		"and vg_fs.fs_name = fs.name "
		"and vg.host_name = '" + hostname + "' "
		"and vg_fs.vg_name like 'bpu%vg' "
		"order by vg.name, fs.name ";
	
	reader = db->ExecuteQuery(sql.c_str());
	while(reader->Read())
	{
		file <<
			"<tr class=\"data\">\n"
			"<td style=\"text-align:left;\">" << reader->GetString(0) << "</td>\n"
			"<td>" << reader->GetDouble(1) << "</td>\n"
			"<td>" << reader->GetDouble(2) << "</td>\n"
			"<td>" << reader->GetDouble(3) << "</td>\n"
			"<td>" << reader->GetDouble(2) / reader->GetDouble(1) * 100.0 << "%</td>\n"
			"<td>" << reader->GetInt(5) << "</td>\n"
			"<td>" << reader->GetDouble(6) << "</td>\n"
			"<td style=\"text-align:left;\">" << reader->GetString(7) << "</td>\n"
			"</tr>\n";
	}
	delete reader;
	
	sql =
		"select sum(vg_fs.lps * vg.pp_size), sum(vg_fs.pps * vg.pp_size)"
		"from vg, vg_fs "
		"where vg.host_name = vg_fs.host_name "
		"and vg.name = vg_fs.vg_name "
		"and vg.host_name = '" + hostname + "' "
		"and vg_fs.vg_name like 'bpu%vg' "
		"and vg_fs.fs_name not in"
		"( "
		"	select fs.name from fs, vg_fs "
		"	where fs.host_name = vg_fs.host_name "
		"	and fs.name = vg_fs.fs_name "
		"	and fs.host_name = '" + hostname + "' "
		"	and vg_fs.vg_name like 'bpu%vg' "
		") ";

	reader = db->ExecuteQuery(sql.c_str());
	while(reader->Read())
	{
		file <<
			"<tr style=\"background-color:#ffccaa;\">"
			"<td style=\"text-align:left;\">Other (jfslog / jfs2log)</td>\n"
			"<td>" << reader->GetDouble(0) << "</td>\n"
			"<td style=\"text-align:center;\">-</td>\n"
			"<td style=\"text-align:center;\">-</td>\n"
			"<td style=\"text-align:center;\">-</td>\n"
			"<td style=\"text-align:center;\">-</td>\n"
			"<td>" << reader->GetDouble(1) << "</td>\n"
			"<td style=\"text-align:center;\">-</td>\n"
			"</tr>\n";
	}
	delete reader;

	sql =
		"select sum(vg_fs.lps * vg.pp_size), sum(vg_fs.pps * vg.pp_size), vg.name "
		"from vg, vg_fs "
		"where vg.host_name = vg_fs.host_name "
		"and vg.name = vg_fs.vg_name "
		"and vg.host_name = '" + hostname + "' "
		"and vg_fs.vg_name like 'bpu%vg' ";

	reader = db->ExecuteQuery(sql.c_str());
	while(reader->Read())
	{
		file <<
			"<tr>\n"
			"<th>Summary</th>\n"
			"<th>" << reader->GetDouble(0) << "</th>\n"
			"<th>-</th>\n"
			"<th>-</th>\n"
			"<th>-</th>\n"
			"<th>-</th>\n"
			"<th>" << reader->GetDouble(1) << "</th>\n"
			"<th>-</th>\n"
			"</tr>\n";
	}
	delete reader;

	sql = "select sum(free_space) from vg where host_name = '" + hostname + "' and name like 'bpu%vg'";

	reader = db->ExecuteQuery(sql.c_str());
	
	while(reader->Read())
	{
		file <<
			"<tr>\n"
			"<th>Expansion</th>\n"
			"<th colspan=\"4\">-</th>\n"
			"<th colspan=\"3\">" << reader->GetDouble(0) << "MB (VG)</th>\n"
			"</tr>\n";
	}
	delete reader;
	
	file <<
		"</table>\n"
		"</td></tr>\n";

	// DataStage volume groups...
	// I am too lazy to write a subroutine to handle different types of fs...
	// It depends on project, may different anywhere.
	file <<
		"<tr><td align=\"center\">\n"
		"<table class=\"report\" cellspacing=\"0\" cellpadding=\"0\" border=\"1\" width=\"100%\">\n"
		"<tr><th>DataStage & File</th><td colspan=\"7\"></td></tr>\n"
		"<tr>\n"
		"<th width=\"31%\" rowspan=\"2\">Filesystem</th>\n"
		"<th colspan=\"4\">Logical</th>\n"
		"<th colspan=\"3\">Physical</th>\n"
		"</tr>\n"
		"<tr>\n"
		"<th width=\"10%\">Size (MB)</th>\n"
		"<th width=\"9%\">Used (MB)</th>\n"
		"<th width=\"9%\">Free (MB)</th>\n"
		"<th width=\"8%\">Used%</th>\n"
		"<th width=\"8%\">Copies</th>\n"
		"<th width=\"10%\">Size (MB)</th>\n"
		"<th width=\"15%\">Volume Group</th>\n"
		"</tr>\n";
	
	sql = 
		"select fs.name, fs.mb_blocks, round(fs.mb_blocks - fs.free, 2), fs.free, fs.pct, "
		"  vg_fs.pps / vg_fs.lps, vg_fs.pps * vg.pp_size, vg.name "
		"from vg, vg_fs, fs "
		"where vg.host_name = vg_fs.host_name "
		"and vg.name = vg_fs.vg_name "
		"and vg_fs.host_name = fs.host_name "
		"and vg_fs.fs_name = fs.name "
		"and vg.host_name = '" + hostname + "' "
		"and vg_fs.vg_name <> 'rootvg' "
		"and vg_fs.vg_name not like 'bpu%vg' "
		"order by vg.name, fs.name ";
	
	reader = db->ExecuteQuery(sql.c_str());
	while(reader->Read())
	{
		file <<
			"<tr class=\"data\">\n"
			"<td style=\"text-align:left;\">" << reader->GetString(0) << "</td>\n"
			"<td>" << reader->GetDouble(1) << "</td>\n"
			"<td>" << reader->GetDouble(2) << "</td>\n"
			"<td>" << reader->GetDouble(3) << "</td>\n"
			"<td>" << reader->GetDouble(2) / reader->GetDouble(1) * 100.0 << "%</td>\n"
			"<td>" << reader->GetInt(5) << "</td>\n"
			"<td>" << reader->GetDouble(6) << "</td>\n"
			"<td style=\"text-align:left;\">" << reader->GetString(7) << "</td>\n"
			"</tr>\n";
	}
	delete reader;
	
	sql =
		"select sum(vg_fs.lps * vg.pp_size), sum(vg_fs.pps * vg.pp_size)"
		"from vg, vg_fs "
		"where vg.host_name = vg_fs.host_name "
		"and vg.name = vg_fs.vg_name "
		"and vg.host_name = '" + hostname + "' "
		"and vg_fs.vg_name <> 'rootvg' "
		"and vg_fs.vg_name not like 'bpu%vg' "
		"and vg_fs.fs_name not in"
		"( "
		"	select fs.name from fs, vg_fs "
		"	where fs.host_name = vg_fs.host_name "
		"	and fs.name = vg_fs.fs_name "
		"	and fs.host_name = '" + hostname + "' "
		"	and vg_fs.vg_name <> 'rootvg' "
		"	and vg_fs.vg_name not like 'bpu%vg' "
		") ";

	reader = db->ExecuteQuery(sql.c_str());
	while(reader->Read())
	{
		file <<
			"<tr style=\"background-color:#ffccaa;\">"
			"<td style=\"text-align:left;\">Other (jfslog / jfs2log)</td>\n"
			"<td>" << reader->GetDouble(0) << "</td>\n"
			"<td style=\"text-align:center;\">-</td>\n"
			"<td style=\"text-align:center;\">-</td>\n"
			"<td style=\"text-align:center;\">-</td>\n"
			"<td style=\"text-align:center;\">-</td>\n"
			"<td>" << reader->GetDouble(1) << "</td>\n"
			"<td style=\"text-align:center;\">-</td>\n"
			"</tr>\n";
	}
	delete reader;

	sql =
		"select sum(vg_fs.lps * vg.pp_size), sum(vg_fs.pps * vg.pp_size), vg.name "
		"from vg, vg_fs "
		"where vg.host_name = vg_fs.host_name "
		"and vg.name = vg_fs.vg_name "
		"and vg.host_name = '" + hostname + "' "
		"and vg_fs.vg_name <> 'rootvg' "
		"and vg_fs.vg_name not like 'bpu%vg' ";

	reader = db->ExecuteQuery(sql.c_str());
	while(reader->Read())
	{
		file <<
			"<tr>\n"
			"<th>Summary</th>\n"
			"<th>" << reader->GetDouble(0) << "</th>\n"
			"<th>-</th>\n"
			"<th>-</th>\n"
			"<th>-</th>\n"
			"<th>-</th>\n"
			"<th>" << reader->GetDouble(1) << "</th>\n"
			"<th>-</th>\n"
			"</tr>\n";
	}
	delete reader;

	sql = "select sum(free_space) from vg where host_name = '" + hostname + "' and name <> 'rootvg' and name not like 'bpu%vg'";

	reader = db->ExecuteQuery(sql.c_str());
	
	while(reader->Read())
	{
		file <<
			"<tr>\n"
			"<th>Expansion</th>\n"
			"<th colspan=\"4\">-</th>\n"
			"<th colspan=\"3\">" << reader->GetDouble(0) << "MB (VG)</th>\n"
			"</tr>\n";
	}
	delete reader;
	
	file <<
		"</table>\n"
		"</td></tr>\n";

	report_html_tail(file);
	
	file.close();
	
	cout << "Generated Report: " << filename << endl;

	return true;
}

bool report_ts_all()
{
	string sql;
	string hostname, instance, database;
	
	SQLite3Reader *reader;
	
	db->Open("cmdw_report.db");
	
	sql = "select distinct host_name, instance, db_name from ts order by host_name, instance, db_name";
	
	reader = db->ExecuteQuery(sql.c_str());	
	while(reader->Read())
	{
		hostname = reader->GetString(0);
		instance = reader->GetString(1);
		database = reader->GetString(2);
		
		report_ts_database(hostname, instance, database);
	}
	delete reader;
	
	db->Close();
	
	// Summary page
	report_ts_summary();
	
	return true;
}

bool report_ts_summary()
{
	string sql;
	string hostname;
	string filename = "./html/DB+Summary.html";
	string title = "CMDW Storage Report - Database Summary";

	SQLite3Reader *reader;
	ofstream file;
	
	file.open(filename.c_str(), ofstream::out);
	file.precision(2);
	file << fixed;

	report_html_head(file, title);
	
	file << "<tr><td class=\"title\">CMDW STORAGE REPORT - DATABASE SUMMARY</td></tr>\n";
	
//	sql = "update ts set host_name='C_LPAR_1' where host_name='C_LPAR_1A'";
//	db->ExecuteUpdate(sql.c_str());
//	sql = "update ts set host_name='C_LPAR_1' where host_name='C_LPAR_1B'";
//	db->ExecuteUpdate(sql.c_str());

	sql = "select distinct host_name from ts order by host_name";
	
	reader = db->ExecuteQuery(sql.c_str());
	while(reader->Read())
	{
		hostname = reader->GetString(0);
		report_ts_summary_host(file, hostname);
	}
	delete reader;
	
	// charts
	report_ts_summary_chart(file);
	
	report_html_tail(file);
	
	file.close();
	
	cout << "Generated Report: " << filename << endl;

	return true;
}

bool report_ts_summary_host(ofstream &file, string hostname)
{
	string sql, sql_db;
	string instance, database;
	
	SQLite3Reader *reader;
	SQLite3Reader *reader_db;

	file << 
		"<tr><td align=\"center\">\n"
		"<table class=\"report\" cellspacing=\"0\" cellpadding=\"0\" border=\"1\" width=\"100%\"> \n"
		"<tr><th width=\"16%\">" + hostname + "</th><td colspan=\"12\"></td></tr>\n"
		"<tr>\n"
		"<th rowspan=\"2\">Database</th>\n"
		"<th colspan=\"4\">Non-partitioned Tablespace (GB)</th>\n"
		"<th colspan=\"4\">Partitioned Tablespace (GB)</th>\n"
		"<th colspan=\"4\">Total (GB)</th>\n"
		"</tr>\n"
		"<tr>\n"
		"<th width=\"7%\">Total</th>\n"
		"<th width=\"7%\">Used</th>\n"
		"<th width=\"7%\">Free</th>\n"
		"<th width=\"7%\">Used %</th>\n"
		"<th width=\"7%\">Total</th>\n"
		"<th width=\"7%\">Used</th>\n"
		"<th width=\"7%\">Free</th>\n"
		"<th width=\"7%\">Used %</th>\n"
		"<th width=\"7%\">Total</th>\n"
		"<th width=\"7%\">Used</th>\n"
		"<th width=\"7%\">Free</th>\n"
		"<th width=\"7%\">Used %</th>\n"
		"</tr>\n";
	
	sql = 
		"select distinct instance, db_name from ts where host_name='" + hostname + "' "
		"order by instance, db_name";
	
	reader = db->ExecuteQuery(sql.c_str());
	while(reader->Read())
	{
		instance = reader->GetString(0);
		database = reader->GetString(1);
		
		// Non-partition TS Summary
		sql_db = 
			"select sum(total_space), sum(total_space - free_space), sum(free_space) "
			"from ts "
			"where host_name = '" + hostname + "' "
			"and   instance = '" + instance + "' "
			"and   db_name = '" + database + "' "
			"and node_num = 0 "
			"and ts_name not like 'TEMPSPACE%'";
			
		reader_db = db->ExecuteQuery(sql_db.c_str());
		while(reader_db->Read())
		{
			string href = "DB+" + hostname + "+" + instance + "+" + database + ".html";
			
			file << 
				"<tr class=\"data\">\n"
				"<td style=\"text-align:left;\"><a href=\"" << href << "\">"<< database << "</a></td>\n"
				"<td>" << reader_db->GetDouble(0) / 1024.0 << "</td>\n"
				"<td>" << reader_db->GetDouble(1) / 1024.0 << "</td>\n"
				"<td>" << reader_db->GetDouble(2) / 1024.0 << "</td>\n";
				
			if(reader_db->GetDouble(0) == 0)
			{
				file << "<td style=\"text-align:center;\">-</td>\n";
			}
			else
			{
				file << "<td>" << reader_db->GetDouble(1) / reader_db->GetDouble(0) * 100.0 << "%</td>\n";
			}
		}
		delete reader_db;
		
		// Partition TS Summary
		sql_db = 
			"select sum(total_space), sum(total_space - free_space), sum(free_space) "
			"from ts "
			"where host_name = '" + hostname + "' "
			"and   instance = '" + instance + "' "
			"and   db_name = '" + database + "' "
			"and node_num <> 0 "
			"and ts_name not like 'TEMPSPACE%'";
			
		reader_db = db->ExecuteQuery(sql_db.c_str());
		while(reader_db->Read())
		{
			file << 
				"<td>" << reader_db->GetDouble(0) / 1024.0 << "</td>\n"
				"<td>" << reader_db->GetDouble(1) / 1024.0 << "</td>\n"
				"<td>" << reader_db->GetDouble(2) / 1024.0 << "</td>\n";
				
			if(reader_db->GetDouble(0) == 0)
			{
				file << "<td style=\"text-align:center;\">-</td>\n";
			}
			else
			{
				file << "<td>" << reader_db->GetDouble(1) / reader_db->GetDouble(0) * 100.0 << "%</td>\n";
			}
		}
		delete reader_db;
		
		// All TS Summary
		sql_db = 
			"select sum(total_space), sum(total_space - free_space), sum(free_space) "
			"from ts "
			"where host_name = '" + hostname + "' "
			"and   instance = '" + instance + "' "
			"and   db_name = '" + database + "' "
			"and ts_name not like 'TEMPSPACE%'";
			
		reader_db = db->ExecuteQuery(sql_db.c_str());
		while(reader_db->Read())
		{
			file << 
				"<td>" << reader_db->GetDouble(0) / 1024.0 << "</td>\n"
				"<td>" << reader_db->GetDouble(1) / 1024.0 << "</td>\n"
				"<td>" << reader_db->GetDouble(2) / 1024.0 << "</td>\n";
				
			if(reader_db->GetDouble(0) == 0)
			{
				file << "<td style=\"text-align:center;\">-</td>\n";
			}
			else
			{
				file << "<td>" << reader_db->GetDouble(1) / reader_db->GetDouble(0) * 100.0 << "%</td>\n";
			}
			
			file << "</tr>\n";
		}
		delete reader_db;		
	}
	delete reader;
	
	// Host Summary
	// Non-partition TS Summary
	sql = 
		"select sum(total_space), sum(total_space - free_space), sum(free_space) "
		"from ts "
		"where host_name = '" + hostname + "' "
		"and node_num = 0 "
		"and ts_name not like 'TEMPSPACE%'";
	
	reader = db->ExecuteQuery(sql.c_str());
	while(reader->Read())
	{
		file <<
			"<tr>\n"
			"<th><b>Summary</b></th>\n"
			"<th>" << reader->GetDouble(0) / 1024.0 << "</th>\n"
			"<th>" << reader->GetDouble(1) / 1024.0 << "</th>\n"
			"<th>" << reader->GetDouble(2) / 1024.0 << "</th>\n"
			"<th>" << reader->GetDouble(1) / reader->GetDouble(0) * 100.0 << "%</th>\n";
	}
	delete reader;
	
	// Partition TS Summary
	sql = 
		"select sum(total_space), sum(total_space - free_space), sum(free_space) "
		"from ts "
		"where host_name = '" + hostname + "' "
		"and node_num <> 0 "
		"and ts_name not like 'TEMPSPACE%'";
	
	reader = db->ExecuteQuery(sql.c_str());
	while(reader->Read())
	{
		file <<
			"<th>" << reader->GetDouble(0) / 1024.0 << "</th>\n"
			"<th>" << reader->GetDouble(1) / 1024.0 << "</th>\n"
			"<th>" << reader->GetDouble(2) / 1024.0 << "</th>\n"
			"<th>" << reader->GetDouble(1) / reader->GetDouble(0) * 100.0 << "%</th>\n";
	}
	delete reader;

	// All TS Summary
	sql = 
		"select sum(total_space), sum(total_space - free_space), sum(free_space) "
		"from ts "
		"where host_name = '" + hostname + "' "
		"and ts_name not like 'TEMPSPACE%'";
	
	reader = db->ExecuteQuery(sql.c_str());
	while(reader->Read())
	{
		file <<
			"<th>" << reader->GetDouble(0) / 1024.0 << "</th>\n"
			"<th>" << reader->GetDouble(1) / 1024.0 << "</th>\n"
			"<th>" << reader->GetDouble(2) / 1024.0 << "</th>\n"
			"<th>" << reader->GetDouble(1) / reader->GetDouble(0) * 100.0 << "%</th>\n"
			"</tr>\n";
	}
	delete reader;
	
	//Expansion space in system
	double fs_ext = 0.0;
	double vg_ext = 0.0;
	double temp = 0.0;
	
	file << "<th>Expansion</th>\n";
	
	sql = "select sum(free) from fs where host_name = '" + hostname + "' and name like '/db2fs%'";
	
	reader = db->ExecuteQuery(sql.c_str());
	while(reader->Read())
	{
		fs_ext = reader->GetDouble(0) / 1024.0;
		temp = reader->GetDouble(0) / 1024.0;
		file << "<th colspan=\"4\">[" << reader->GetDouble(0) / 1024.0 << "GB (FS) + ";
	}
	delete reader;
	
	sql = "select sum(free_space) from vg where host_name = '" + hostname + "' and name like 'bpu%'";
	reader = db->ExecuteQuery(sql.c_str());
	while(reader->Read())
	{
		vg_ext += reader->GetDouble(0) / 1024.0;
		temp += reader->GetDouble(0) / 1024.0;
		file << reader->GetDouble(0) / 1024.0 << "GB (VG)]<br>" << temp << "GB</th>\n";		
	}
	delete reader;
	
	sql = 
		"select sum(free) from fs where host_name not like '" + hostname.substr(0, 1) + "%1%' "
		"and host_name like '" + hostname.substr(0, 1) + "%' "
		"and name like '/db2fs%'";
	
	reader = db->ExecuteQuery(sql.c_str());
	while(reader->Read())
	{
		fs_ext += reader->GetDouble(0) / 1024.0;
		temp = reader->GetDouble(0) / 1024.0;
		file << "<th colspan=\"4\">[" << reader->GetDouble(0) / 1024.0 << "GB (FS) + ";
	}
	delete reader;
	
	sql = 
		"select sum(free_space) from vg where host_name not like '" + hostname.substr(0, 1) + "%1%' "
		"and host_name like '" + hostname.substr(0, 1) + "%' "
		"and name like 'bpu%'";
	
	reader = db->ExecuteQuery(sql.c_str());
	while(reader->Read())
	{
		vg_ext += reader->GetDouble(0) / 1024.0;
		temp += reader->GetDouble(0) / 1024.0;
		file << reader->GetDouble(0) / 1024.0 << "GB (VG)]<br>" << temp << "GB</th>\n";		
	}
	delete reader;
	
	file << 
		"<th colspan=\"4\">[" << fs_ext << "GB (FS) + " << vg_ext << "GB (VG)]<br>" << fs_ext + vg_ext << "GB</th>\n"
		"</tr>\n";
	
	if(hostname[0] == 'C')
	{
		file << 
			"<tr>\n"
			"<th></th>\n"
			"<th colspan=\"12\" style=\"text-align:left;color:#ff0000\">"
			"Partitioned Tablespace expansion is shared between C_LPAR_1A & C_LPAR_1B"
			"</th>"
			"</tr>\n";
	}

	file << "</table>\n</td></tr>\n";
	
	return true;
}

bool report_ts_summary_chart(ofstream &file)
{
	string sql;
	string hostname, instance, database;
	double total_space;
	double used_space;
	
	//Page with 902, pad left/right 20, first 2 cols 71x2
	int chart_width = 720;
	int scale = 10;
	
	SQLite3Reader *reader;
	
	sql = 
		"select max(total) from "
		"( "
		"	select host_name, instance, db_name, sum(total_space) as total "
		"	from ts "
		"	where ts_name not like 'TEMPSPACE%' "
		"	group by host_name, instance, db_name "
		") ";
	
	reader = db->ExecuteQuery(sql.c_str());
	while(reader->Read())
	{
		scale = int(reader->GetDouble(0) / 1024.0 / 1024.0) + 1;
		
		//adjust scale
		if(scale == 7 || scale == 11 || scale == 13 || scale == 14 || scale == 17 || scale == 19)
		{
			scale++;
			if(scale == 14) scale++;
		}
	}
	delete reader;
	
	file <<
		"<tr><td>\n"
		"<table class=\"report\" cellspacing=\"0\" cellpadding=\"0\" border=\"1\" width=\"100%\"> \n"
		"<tr><th width=\"20%\">Tablespace Chart</th></tr>\n"
		"<tr>\n"
		"<td>\n"
		"<center>\n"
		"<table>\n"
		"<tr>\n"
		"<td style=\"border:#ffffff;\" width=\"815\">Total Space<br>Used Space</td>\n"
		"<td width=\"60\">\n"
		"<div class=\"total\" style=\"width:60px;\"></div>\n"
		"<div class=\"used\" style=\"width:60px;\"></div>\n"
		"</td>\n"
		"</tr>\n"
		"</table>\n"
		"<table class=\"report\">\n"
		"<tr>\n"
		"<th width=\"90px\">Server /<br>Database</th>\n"
		"<th width=\"70px\">Total /<br>Used</th>\n";
	
	for(int i = 1; i <= scale; i++)
	{
		file << "<th style=\"text-align:right;\" width=\"" << int(chart_width/scale) << "px\">" << i << "TB</th>\n";
	}
	
	file << "</tr>\n";

	sql =
		"select host_name, instance, db_name, sum(total_space), sum(total_space - free_space) as total "
		"from ts "
		"where ts_name not like 'TEMPSPACE%' "
		"group by host_name, instance, db_name ";
	
	reader = db->ExecuteQuery(sql.c_str());
	while(reader->Read())
	{
		hostname = reader->GetString(0);
		instance = reader->GetString(1);
		database = reader->GetString(2);
		total_space = reader->GetDouble(3) / 1024.0 / 1024.0;
		used_space = reader->GetDouble(4) / 1024.0 / 1024.0;
		
		file <<		
			"<tr>\n"
			"<th style=\"text-align:center;\">" << hostname << "<br>" << database << "</th>\n"
			"<td style=\"text-align:center;\">" << total_space << "TB<br>" << used_space << "TB</td>\n"
			"<td colspan=\"" << scale << "\" style=\"text-align:left;padding-left:0px;\">\n"
			"<div class=\"total\" style=\"width:" << int(total_space / scale * chart_width) + 1 << "px;\"></div>\n"
			"<div class=\"used\" style=\"width:" << int(used_space / scale * chart_width) + 1 << "px;\"></div>\n"
			"</td>\n"
			"</tr>\n";
	}
	delete reader;
	
	file <<
		"</table>\n"
		"</center>\n"
		"</td>\n"
		"</tr>\n"
		"</table>\n"
		"</td></tr>\n";
	
	return true;
}

bool report_ts_database(string hostname, string instance, string database)
{
	string sql;
	string filename = "./html/DB+" + hostname + "+" + instance + "+" + database + ".html";
	string title = "FMS CMDW Storage Report: Database+" + hostname + "+" + instance + "+" + database;
	
	SQLite3Reader *reader;
	ofstream file;
	
	file.open(filename.c_str(), ofstream::out);
	file.precision(2);
	file << fixed;
	
	report_html_head(file, title);
	
	// Report head
	file << 
		"<tr><td class=\"title\">CMDW STORAGE REPORT - DATABASE</td></tr>\n"
		"<tr><td style=\"text-align:left;\">Server: <b>" << hostname << "</b>&nbsp;&nbsp;"
		"DB2 Instance: <b>" << instance << "</b>&nbsp;&nbsp;"
		"Database: <b>" << database << "</b></td></tr>\n";
	
	// Table for Non-partitioned Tablespaces
	file <<	
		"<tr><td align=\"center\">\n"
		"<table class=\"report\" cellspacing=\"0\" cellpadding=\"0\" border=\"1\" width=\"100%\"> \n"
		"<tr><th>Non-partitioned Tablespace</th><td colspan=\"5\"></td></tr>\n"
		"<tr>\n"
		"<th width=\"25%\">Table Space</th>\n"
		"<th width=\"15%\">Total Space (MB)</th>\n"
		"<th width=\"15%\">Used Space (MB)</th>\n"
		"<th width=\"15%\">Free Space (MB)</th>\n"
		"<th width=\"15%\">Used %</th>\n"
		"<th width=\"15%\">Partition Count</th>\n"
		"</tr>\n";
	
	// Non-partitioned Tablespaces details
	sql = 
		"select ts_name, sum(total_space), sum(total_space - free_space), "
		"       sum(free_space), count(*) as par_cnt "
		"from  ts "
		"where host_name = '" + hostname + "' "
		"and   instance = '" + instance + "' "
		"and   db_name = '" + database + "' "
		"and   node_num = 0 "
		"and   ts_name not like 'TEMPSPACE%' "
		"group by ts_name, page_size "
		"order by ts_name ";
	
	reader = db->ExecuteQuery(sql.c_str());
	while(reader->Read())
	{
		file <<
			"<tr class=\"data\">\n"
			"<td style=\"text-align:left;\">" << reader->GetString(0) << "</td>\n"
			"<td>" << reader->GetDouble(1) << "</td>\n"
			"<td>" << reader->GetDouble(2) << "</td>\n"
			"<td>" << reader->GetDouble(3) << "</td>\n";
			
		if(reader->GetDouble(1) == 0)
		{
			file << "<td style=\"text-align:center;\">-</td>\n";
		}
		else
		{
			file << "<td>" << reader->GetDouble(2) / reader->GetDouble(1) * 100.0 << "%</td>\n";
		}
		
		file <<
			"<td>" << reader->GetString(4) << "</td>\n" <<
			"</tr>\n";
	}
	delete reader;
	
	//Non-partitioned Tablespaces Summary
	sql = 
		"select sum(total_space), sum(total_space - free_space), sum(free_space) "
		"from ts "
		"where host_name = '" + hostname + "' "
		"and instance = '" + instance + "' "
		"and db_name = '" + database + "' "
		"and node_num = 0 "
		"and ts_name not like 'TEMPSPACE%'";
	
	reader = db->ExecuteQuery(sql.c_str());
	while(reader->Read())
	{
		file <<
			"<tr>\n"
			"<th><b>Summary</b></th>\n"
			"<th>" << reader->GetDouble(0) << "MB</th>\n"
			"<th>" << reader->GetDouble(1) << "MB</th>\n"
			"<th>" << reader->GetDouble(2) << "MB</th>\n";
			
		if(reader->GetDouble(0) == 0)
		{
			file << "<th>-</th>\n";
		}
		else
		{
			file << "<th>" << reader->GetDouble(1) / reader->GetDouble(0) * 100.0 << "%</th>\n";
		}
		
		file <<
			"<th>-</th>\n" <<
			"</tr>\n";
	}
	delete reader;
	
	file << "</table>\n</td></tr>\n";
	
	// Table for Partitioned Tablespaces
	file <<	
		"<tr><td align=\"center\">\n"
		"<table class=\"report\" cellspacing=\"0\" cellpadding=\"0\" border=\"1\" width=\"100%\"> \n"
		"<tr><th>Partitioned Tablespace</th><td colspan=\"5\"></td></tr>\n"
		"<tr>\n"
		"<th width=\"25%\">Table Space</th>\n"
		"<th width=\"15%\">Total Space (MB)</th>\n"
		"<th width=\"15%\">Used Space (MB)</th>\n"
		"<th width=\"15%\">Free Space (MB)</th>\n"
		"<th width=\"15%\">Used %</th>\n"
		"<th width=\"15%\">Partition Count</th>\n"
		"</tr>\n";
	
	// Partitioned Tablespaces details
	sql = 
		"select ts_name, sum(total_space), sum(total_space - free_space), "
		"  sum(free_space), count(*) as par_cnt "
		"from ts "
		"where host_name = '" + hostname + "' "
		"and instance = '" + instance + "' "
		"and db_name = '" + database + "' "
		"and node_num <> 0 "
		"and ts_name not like 'TEMPSPACE%' "
		"group by ts_name, page_size "
		"order by ts_name ";

	reader = db->ExecuteQuery(sql.c_str());
	while(reader->Read())
	{
		file <<
			"<tr class=\"data\">\n"
			"<td style=\"text-align:left;\">" << reader->GetString(0) << "</td>\n"
			"<td>" << reader->GetDouble(1) << "</td>\n"
			"<td>" << reader->GetDouble(2) << "</td>\n"
			"<td>" << reader->GetDouble(3) << "</td>\n";
			
		if(reader->GetDouble(1) == 0)
		{
			file << "<td style=\"text-align:center;\">-</td>\n";
		}
		else
		{
			file << "<td>" << reader->GetDouble(2) / reader->GetDouble(1) * 100.0 << "%</td>\n";
		}
		
		file <<
			"<td>" << reader->GetString(4) << "</td>\n" <<
			"</tr>\n";
	}
	delete reader;

	//Partitioned Tablespaces Summary
	sql = 
		"select sum(total_space), sum(total_space - free_space), sum(free_space) "
		"from ts "
		"where host_name = '" + hostname + "' "
		"and instance = '" + instance + "' "
		"and db_name = '" + database + "' "
		"and node_num <> 0 "
		"and ts_name not like 'TEMPSPACE%'";
	
	reader = db->ExecuteQuery(sql.c_str());
	while(reader->Read())
	{
		file <<
			"<tr>\n"
			"<th><b>Summary</b></th>\n"
			"<th>" << reader->GetDouble(0) << "MB</th>\n"
			"<th>" << reader->GetDouble(1) << "MB</th>\n"
			"<th>" << reader->GetDouble(2) << "MB</th>\n";
			
		if(reader->GetDouble(0) == 0)
		{
			file << "<th>-</th>\n";
		}
		else
		{
			file << "<th>" << reader->GetDouble(1) / reader->GetDouble(0) * 100.0 << "%</th>\n";
		}
		
		file <<	"<th>-</th>\n</tr>\n";
	}
	delete reader;
	
	file << "</table>\n</td></tr>\n";

	report_html_tail(file);
	
	file.close();
	
	cout << "Generated Report: " << filename << endl;

	return true;
}

void report_index()
{
	string sql;
	string filename = "./html/index.html";
	string link;
	int count;

	ofstream file;
	SQLite3Reader *reader;
	
	file.open(filename.c_str(), ofstream::out);
	
	report_html_head(file, string("FMS CMDW Storage Report"));

	file <<
		"<tr><td class=\"title\">CMDW STORAGE REPORT</td></tr>\n"
		"<tr><td>\n"
		"<table class=\"report\" cellspacing=\"0\" cellpadding=\"0\" border=\"1\" width=\"100%\">\n"
		"<tr><th style=\"text-align:left;\" colspan=\"3\">Storage</th></tr>\n"
		"<tr class=\"data\">\n"
		"<td style=\"text-align:left;\" width=\"33%\"><a href=\"STG+Summary.html\">Storage Summary</a></td>\n";
	
	count = 1;

	sql = "select distinct host_name from vg order by host_name";
	
	reader = db->ExecuteQuery(sql.c_str());
	while(reader->Read())
	{
		link = string("STG+") + reader->GetString(0) + ".html";
		file <<
			"<td style=\"text-align:left;\" width=\"33%\"><a href=\"" << link << "\">" << 
			reader->GetString(0) << "</a></td>\n";
		
		count++;
		if(count % 3 == 0)
		{
			file << "</tr>\n<tr class=\"data\">\n";
		}
	}
	delete reader;

	count = 3 - count % 3;
	if(count < 3)
	{
		for(int i = 0; i < count; i++)
		{
			file << "<td style=\"text-align:left;\" width=\"33%\"></td>\n";
		}
	}
	file << "</tr>";

	file <<
		"</table>\n"
		"</td></tr>\n"
		"<tr><td>\n"
		"<table class=\"report\" cellspacing=\"0\" cellpadding=\"0\" border=\"1\" width=\"100%\">\n"
		"<tr><th style=\"text-align:left;\" colspan=\"3\">Database</th></tr>\n"
		"<tr class=\"data\">\n"
		"<td style=\"text-align:left;\" width=\"33%\"><a href=\"DB+Summary.html\">Database Summary</a></td>\n";

	count = 1;

	sql = "select distinct host_name, instance, db_name from ts order by host_name, instance, db_name";
	
	reader = db->ExecuteQuery(sql.c_str());
	while(reader->Read())
	{
		link = string("DB+") + reader->GetString(0) + "+" + reader->GetString(1) + "+" + reader->GetString(2) + ".html";
		file <<
			"<td style=\"text-align:left;\" width=\"33%\"><a href=\"" << link << "\">" << 
			reader->GetString(0) << " " << 
			reader->GetString(1) << " " << 
			reader->GetString(2) << "</a></td>\n";
			
		count++;
		if(count % 3 == 0)
		{
			file << "</tr>\n<tr class=\"data\">\n";
		}
	}
	delete reader;
	
	count = 3 - count % 3;
	if(count < 3)
	{
		for(int i = 0; i < count; i++)
		{
			file << "<td style=\"text-align:left;\" width=\"33%\"></td>\n";
		}
	}
	file << "</tr>";

	file <<
		"</table>\n"
		"</td></tr>\n";
	
	report_html_tail(file);
	
	cout << "Generated Report Index Page" << endl;
}

void report_history()
{
	string filename = "./html/history.html";

	ofstream file;
	
	file.open(filename.c_str(), ofstream::out);
	
	report_html_head(file, string("FMS CMDW Storage Report - History"));

	file <<
		"<tr><td class=\"title\">CMDW STORAGE REPORT - History</td></tr>\n"
		"<tr><td>\n"
		"<table class=\"report\" cellspacing=\"0\" cellpadding=\"0\" border=\"1\" width=\"100%\">\n"
		"<tr><th style=\"text-align:left;\">History</th></tr>\n"
		"<tr class=\"data\"><td style=\"text-align:left;\"><a href=\"index.html\">Latest</a></td></tr>\n"
		"</table>\n"
		"</td></tr>\n";
	
	report_html_tail(file);
	
	cout << "Generated Report History Page" << endl;
}

void report_html_head(ofstream &file, string title)
{
	string sql = "select time from time";
	string time = "";
	
	SQLite3Reader *reader;
	
	db->Open("cmdw_report.db");
	
	reader = db->ExecuteQuery(sql.c_str());
	while(reader->Read())
	{
		time = reader->GetString(0);
	}
	delete reader;
	
	db->Close();
	
	file <<
		"<!DOCTYPE html PUBLIC \"-//W3C//DTD XHTML 1.0 Transitional//EN\" \"http://www.w3.org/TR/xhtml1/DTD/xhtml1-transitional.dtd\">\n"
		"<html lang=\"en-US\" xml:lang=\"en-US\" xmlns=\"http://www.w3.org/1999/xhtml\">\n"
		"<head>\n"
		"<title>" << title << "</title>\n"
		"<style>\n"
		"body,table,td,th {font-family:verdana,helvetica,arial,sans-serif;}\n"
		"body {font-size:70%;color:#000000;background-color:#f1f1ed;margin:0px;overflow:scroll;}\n"
		"table,th,td {font-size:100%;}\n"
		"table.top {width:902px;margin-top:0px;margin-bottom:0px;background-color:#ffffff;}\n"
		"table.report {border:1px solid #c3c3c3;border-collapse:collapse;}\n"
		"table.report th {background-color:#e5eecc;border:1px solid #c3c3c3;padding:3px;}\n"
		"table.report td {border:1px solid #c3c3c3;padding:3px;text-align:right;}\n"
		"table.report td {font-size:12px;font-family:courier new,verdana,helvetica,arial,sans-serif;}\n"
		"div.total {background-color:#99aaff;text-align:right;height:12px;}\n"
		"div.used  {background-color:#ffaa99;text-align:right;height:12px;}\n"
		"tr.data {background-color:#ffffff;}\n"
		"tr.gray {background-color:#f7f7f7;}\n"
		"a.contact:link {color: #999999; text-decoration: none}\n"
		"a.contact:visited {color: #999999; text-decoration: none}\n"
		"a.contact:hover {text-decoration: underline}\n"
		".title {font-size:200%;text-align:left;}\n"
		"</style>\n"
		"<script type=\"text/javascript\">\n"
		"function colorit(){var arr,i,flip=0;arr=document.getElementsByTagName('tr');\n"
		"for(i = 0;i<arr.length;i++){if(arr[i].className=='data'){if(flip == 0){\n"
		"flip = 1;}else{arr[i].className='gray';flip = 0;}}}}\n"
		"</script>\n"
		"</head>\n"
		"<body onload=\"colorit()\">\n"
		"<center>\n"
		"<table class=\"top\" border=\"0\" cellpadding=\"6\" cellspacing=\"0\">\n"
		"<tr><td>\n"
		"<table class=\"report\" cellspacing=\"0\" cellpadding=\"0\" border=\"1\" width=\"100%\">\n"
		"<tr>\n"
		"<th width=\"10%\"><a href=\"index.html\">Home</a></th>\n"
		"<th width=\"10%\"><a href=\"STG+Summary.html\">Storage</a></th>\n"
		"<th width=\"10%\"><a href=\"DB+Summary.html\">Database</a></th>\n"
		"<th width=\"10%\"><a href=\"history.html\">History</a></th>\n"
		"<th width=\"60%\" style=\"text-align:right\">Report Generated on " << time << "</th>\n"
		"</tr>\n"
		"</table>\n"
		"</td></tr>\n";
}

void report_html_tail(ofstream &file)
{
	file <<	
		"<tr><td>\n"
		"<table class=\"report\" cellspacing=\"0\" cellpadding=\"0\" border=\"1\" width=\"100%\">\n"
		"<tr><th style=\"color:#999999\">Powered by FMS Infrastructure Team, IBM Corporation. &copy;2009<br>\n"
		"All suggestions are welcome! Looking for more infomation? "
		"<a href=\"mailto:mingjieli@cn.ibm.com\" class=\"contact\">Contact us.</a></th></tr>\n"
		"</table>\n"
		"</td></tr>\n"
		"</table>\n"
		"<center>\n"
		"</body>\n"
		"</html>\n";
}
