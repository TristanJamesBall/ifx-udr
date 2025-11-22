#!/bin/ksh

export DBACCESS_COLUMNS=${COLUMNS:-100}

printf "\n============== $(basename $0) ==================\n"
DBNAME=tjb
FIFO="./.timing.fifo"
export FET_BUF_SIZE=$(( 1024 * 256 ))

if which pv >/dev/null 2>&1 ; then
	pv() {
		label="${1:-Lines/Sec}"
		command pv --discard --line-mode --format "${label} - Rate: %r  Count: %b  Elapsed: %t"
	}
else
	pv() {
		wc -l 
	}
fi

trap 'rm -f $FIFO; trap - EXIT HUP TRAP; exit' EXIT HUP INT

rm -f $FIFO
mkfifo $FIFO

printf "\n================== OUTPUT DEMO ==================\n\n"

## Very rough start barrier, the before /after times still
## won't match of course, but we'll probably get them the 
## same to the first decimal place
##
## Bash is too slow for this, ksh only.

typeset -i -s msec=0
while (( msec != 58 )); do
	printf -v msec "%(%2N)T" @now
done

printf "Before_Date       %(%F %T.%3N)T\n\n" @now 
{	
	## Eyeball test / Demo

	## If you can set INFORMIXSERVER to a shm or ipcstr connection type
	## and run this on the server itelf, the before/afte times here will
	## be close to the times output from the sql

	## Other similar things before you run the script
	##
	## echo 'performance' | tee /sys/devices/system/cpu/cpufreq/policy*/scaling_governors
	##
	## sudo renice -10 $$
	## Make sure we run on a performance, not efficencty core. Your cores will differ!
	## taskset -pc 0,1 $$
	
	## But none of that really matters - it'll be blindingly obvious if the times are
	## wrong


	dbaccess ${DBNAME} <<-EOF
		select
			current             as current,
			sysdate             as sysdate,
			realtime_dt()       as realtime_dt,
			utc_realtime_dt()   as utc_realtime_dt,
			utc_realtime_dec()::lvarchar(45)  as utc_realtime_dec
		from
			sysmaster:sysdual
		into
			temp t1;

		select * from t1;
	EOF
} 2>&1 |grep -E -v '(^($|Database)|retrieved)'
printf "\nAfter_Date        %(%F %T.%3N)T\n" @now 


export TIMEFORMAT="Elapsed: %3R"

printf "\n================== Fixed String Rate Tests ==================\n\n" 

for DT in  'current' 'sysdate' 'realtime_dt()' 'utc_realtime_dt()' 'utc_realtime_dec()'  ; do

	label="$( printf "%-35.35s" "'$DT' Lines/Sec ........................................." )"

	pv "$label" < $FIFO  &

	# "::char(26)||'X' " gives us a fixed string size for all commands"

	dbaccess $DBNAME 2>/dev/null  <<-EOF

	unload to $FIFO
	select
		${DT}::char(26)||'X' as now
    from
                   table( list{1,2,3,4,5,6,7,8,9,9} )
        cross join table( list{1,2,3,4,5,6,7,8,9,0} )
        cross join table( list{1,2,3,4,5,6,7,8,9,0} )
        cross join table( list{1,2,3,4,5,6,7,8,9,0} )
        cross join table( list{1,2,3,4,5,6,7,8,9,0} )
        cross join table( list{1,2,3,4,5,6,7,8,9,0} )
        cross join table( list{1,2,3,4,5,6,7,8,9,0} )
	;
EOF
done


printf "\n================== Raw Rate Tests ==================\n\n" 

for DT in  'current' 'sysdate' 'realtime_dt()' 'utc_realtime_dt()' 'utc_realtime_dec()'  ; do

	label="$( printf "%-35.35s" "'$DT' Lines/Sec ........................................." )"

	pv "$label" < $FIFO  &


	dbaccess $DBNAME 2>/dev/null  <<-EOF

	unload to $FIFO
	select
		${DT} as now
    from
                   table( list{1,2,3,4,5,6,7,8,9,9} )
        cross join table( list{1,2,3,4,5,6,7,8,9,0} )
        cross join table( list{1,2,3,4,5,6,7,8,9,0} )
        cross join table( list{1,2,3,4,5,6,7,8,9,0} )
        cross join table( list{1,2,3,4,5,6,7,8,9,0} )
        cross join table( list{1,2,3,4,5,6,7,8,9,0} )
        cross join table( list{1,2,3,4,5,6,7,8,9,0} )
	;
EOF
	(( $? != 0 )) && exit
done

