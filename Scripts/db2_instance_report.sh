#!/bin/ksh

. ~`whoami`/.profile

INSTANCE=`whoami`
HOST_NAME=`hostname`

printf "TYPE     Host_Name  INSTANCE DB_NAME    #   TS_NAME            TOT_SPC_MB  USED_SPC_MB FREE_SPC_MB DEC_MAX_MB  DEC_MAX_PPP PG_SZ_KB PAR_CNT\n"
printf "-------- ---------- -------- ---------- --- ------------------ ----------- ----------- ----------- ----------- ----------- -------- -------\n"

DB_NAME_LIST=`db2 list db directory | grep -ip ind | grep 'Database alias' | awk '{ print $4}'`

for DB_NAME in $DB_NAME_LIST
do

  db2 +o connect to $DB_NAME
  NODELIST=`db2 -tx select distinct DBPARTITIONNUM from SYSCAT.DBPARTITIONGROUPDEF order by DBPARTITIONNUM`
  db2 +o connect reset

  for NODE in $NODELIST
  do

    FIRSTCOLS=`printf "#TS#     %-10s %-8s %-10s %3s" $HOST_NAME $INSTANCE $DB_NAME $NODE`

    db2 +o set client connect_dbpartitionnum $NODE
    db2 +o connect to $DB_NAME
    db2 -tx \
    " \
      select \
        TS_NAME, TOT_SPC_MB, USED_SPC_MB, FREE_SPC_MB, DEC_MAX_MB, DEC_MAX_PPP, \
        smallint(PAGESIZE / 1024) as PG_SZ_KB, \
        smallint(PAR_CNT) as PAR_CNT \
      from \
      ( \
        select \
          substr(TABLESPACE_NAME, 1, 18) AS TS_NAME, \
          decimal(sum((float(TOTAL_PAGES) / 1024.0) * (float(PAGE_SIZE) / 1024.0)), 9, 2) as TOT_SPC_MB, \
          decimal(sum((float(USED_PAGES)  / 1024.0) * (float(PAGE_SIZE) / 1024.0)), 9, 2) as USED_SPC_MB, \
          decimal(sum((float(FREE_PAGES)  / 1024.0) * (float(PAGE_SIZE) / 1024.0)), 9, 2) as FREE_SPC_MB, \
          decimal(min((float(USABLE_PAGES - HIGH_WATER_MARK) / 1024.0) * (float(PAGE_SIZE) / 1024.0)) * count(*), 9, 2) as DEC_MAX_MB, \
          integer(min(USABLE_PAGES - HIGH_WATER_MARK)) as DEC_MAX_PPP, \
          count(*) as PAR_CNT \
        from table(sysproc.snapshot_tbs_cfg('$DB_NAME', -1)) as t \
        group by TABLESPACE_NAME \
        order by TABLESPACE_NAME \
      ) ss, \
      syscat.tablespaces ts \
      where ss.TS_NAME = ts.TBSPACE \
      ; \
    " | sed -e 's/\(.*\)/'"$FIRSTCOLS"' \1/g'

    db2 +o connect reset

  done;

done;

