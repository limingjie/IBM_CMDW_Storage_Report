#!/bin/ksh

printf "TYPE     Host_Name  VG_Name    LV NAME             TYPE       LPs     PPs     PVs  LV STATE      MOUNT POINT\n"
printf "-------- ---------- ---------- ------------------- ---------- ------- ------- ---- ------------- -----------\n"

HOST_NAME=`hostname`

for VG in `lsvg`
do

  FIRSTCOLS=`printf "#VGFS#   %-10s %-10s" $HOST_NAME $VG`
  lsvg -l $VG | sed '1,2 d' | sed -e 's/\(.*\)/'"$FIRSTCOLS"' \1/g'
  
done

