#!/bin/ksh

printf "TYPE     Host_Name  VG_Name    VG_Space(MB)    Free_Space(MB)  Used_Space(MB)  PP_Size(MB) \n"
printf "-------- ---------- ---------- --------------- --------------- --------------- ------------\n"

HOST_NAME=`hostname`

for VG in `lsvg`;
do

  TOTAL_PPS=`lsvg $VG | grep "TOTAL PPs" | awk '{ print $6 }'`
  FREE_PPS=`lsvg $VG | grep "FREE PPs" | awk '{ print $6 }'`
  USED_PPS=`lsvg $VG | grep "USED PPs" | awk '{ print $5 }'`
  PP_SIZE_MB=`lsvg $VG | grep "PP SIZE" | awk '{ print $6 }'`

  VG_SPC=`echo $TOTAL_PPS" * "$PP_SIZE_MB | bc`
  FREE_SPC=`echo $FREE_PPS" * "$PP_SIZE_MB | bc`
  USED_SPC=`echo $USED_PPS" * "$PP_SIZE_MB | bc`

  printf "#VG#     %-10s %-10s %15s %15s %15s %12s\n" $HOST_NAME $VG $VG_SPC $FREE_SPC $USED_SPC $PP_SIZE_MB

done

