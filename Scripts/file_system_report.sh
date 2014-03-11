#!/bin/ksh

printf "TYPE     Host_Name  Filesystem           MB blocks       Free            PCT   Mounted on                                        \n"
printf "-------- ---------- -------------------- --------------- --------------- ----- --------------------------------------------------\n"

HOST_NAME=`hostname`
FIRSTCOLS=`printf "#FS#     %-10s" $HOST_NAME`

df -m | grep ^/ | awk '{ printf "%-20s %15s %15s %5s %-50s\n", $1, $2, $3, $4, $7}' | sed -e 's/\(.*\)/'"$FIRSTCOLS"' \1/g'

