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

for cmd in bop_left ifx_bit_leftshift ; do

	pv < $FIFO &

	dbaccess $DBNAME 2>/dev/null  <<-EOF

	select
		(row_number() over ())::bigint as rn
    from
                   table( list{1,2,3,4,5,6,7,8,9,9} )
        cross join table( list{1,2,3,4,5,6,7,8,9,0} )
        cross join table( list{1,2,3,4,5,6,7,8,9,0} )
        cross join table( list{1,2,3,4,5,6,7,8,9,0} )
        cross join table( list{1,2,3,4,5,6,7,8,9,0} )
        cross join table( list{1,2,3,4,5,6,7,8,9,0} )
        cross join table( list{1,2,3,4,5,6,7,8,9,0} )
	into temp t1
	;

	unload to $FIFO
	select

		rn as orig,
		mod(rn,62)::smallint shift,
		${cmd}( rn, mod(rn,62)::smallint) as result

	from t1;

	}
	
EOF
done

