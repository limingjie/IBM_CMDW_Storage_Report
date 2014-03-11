#!/bin/ksh

DB2_INST_LIST=`lsuser -a ALL | grep db2inst | sort`

for DB2_INST in $DB2_INST_LIST
do

  echo "Working on" $DB2_INST "..."
  sudo -u $DB2_INST /martin/storage_report/db2_instance_report.sh
  echo "Done on" $DB2_INST
  
done;

